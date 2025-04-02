#include "pch.h"
#include <istream>
#include <iostream>
#include <sstream>
#include <WS2tcpip.h>
#include <webstur/ip/tcp/tcpclient.h>

void TCPClient::request(char* payload, int payload_size) {
	// Подключиться к серверу
	this->start();
	// Отправить сообщение
	auto request_stream = std::istringstream(payload != nullptr ? 
		std::string(payload, payload + payload_size) : "");
	this->sendMessage(request_stream);
}

void TCPClient::shutdown() {
	if (!(*this->running)) {
		std::clog << "UDPClient is already stopped" << std::endl;
		return;
	}

	std::clog << "Stopping client" << std::endl;
	// Указываем что клиенту нужно приостановиться
	// и ждём пока он остановится
	*this->should_run = false;
	waitForClientStop();
	delete this->current_runner;
}

TCPClient::TCPClient(std::string address, int port) : server_address(address), server_port(port) {
	this->should_run = new std::atomic<bool>(false);
	this->running = new std::atomic<bool>(false);

	std::clog << "Creating socket" << std::endl;

	// Создаём сокет
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP
	);

	std::clog << "Creating socket bind addr" << std::endl;
	// Создаём адрес к которому привяжется сокет
	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr = getDeviceAddrInfo().sin_addr;
	bind_addr.sin_port = 0;

	std::clog << "Binding socket" << std::endl;
	// Привязать сокет к адресу
	if (bind(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to bind socket descriptor"));
}

TCPClient::~TCPClient() {
	std::clog << "Stopping worker thread" << std::endl;
	// Приостанавливаем рабочий поток
	this->shutdown();

	std::clog << "Closing socket" << std::endl;
	// Закрываем сокет
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));

	delete this->should_run;
	delete this->running;
}

void TCPClient::waitForClientStop() {
	while (*this->running) {
	}
}

void TCPClient::sendMessage(std::istream& message) {
	char buffer[TCP_MAX_MESSAGE_SIZE];

	// Отправить данные клиенту
	while (!message.eof()) {
		auto bytes_read = message.readsome(buffer, sizeof(buffer));

		if (bytes_read == 0) break;

		if (send(this->socket_descriptor, buffer, bytes_read, 0) == SOCKET_ERROR)
			throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't send data over TCP"));
	}
}

void TCPClient::start() {
	// Если клиент уже работает, выходим из него
	if (*this->running) {
		std::clog << "UDPClient is already running" << std::endl;
		return;
	}

	std::clog << "Starting client" << std::endl;
	// Подготавливаем рабочий поток
	delete this->current_runner;
	*this->should_run = true;
	this->current_runner = new std::thread(&TCPClient::connection, this);
	this->current_runner->detach();
}

std::ostream& TCPClient::printClientInfo(std::ostream& out) {
	sockaddr_in client_address;
	int client_address_size = sizeof(client_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&client_address, &client_address_size);

	out << "IP client info:\n" <<
		"TCPClient state: " << (*this->running ? "running" : "not running") << "\n";

	if (get_sock_name_res == SOCKET_ERROR) {
		out << "Unable to get socket info\n";
	}
	else {
		out << "Socket info:\n";
		printSockaddrInfo(out, client_address);
		out << "\n";
	}

	out.flush();

	return out;
}

void TCPClient::connection() {
	std::clog << "Starting TCP client" << std::endl;
	// Запуск TCP-сервера

	// Устанавливаем флаг работы в true
	*this->running = true;

	try {
		sockaddr_in bind_addr;
		inet_pton(AF_INET, &this->server_address[0], &(bind_addr.sin_addr));
		bind_addr.sin_family = AF_INET;
		bind_addr.sin_port = htons(this->server_port);

		// Подключаемся к серверу
		if (connect(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
			throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't connect to server at " +
				this->server_address + ":" + std::to_string(this->server_port)));

		// Оповещаем при помощи метода onConnect что получено новое соединение
		this->onConnect();

		std::vector<char> buffer(0, TCP_MAX_MESSAGE_SIZE);
		FD_SET readfds;
		timeval timeout;
		timeout.tv_sec = TCP_SERVER_TIMEOUT_S;
		timeout.tv_usec = 0;
		bool outer_close = false;
		while (*this->should_run) {
			FD_ZERO(&readfds);
			FD_SET(this->socket_descriptor, &readfds);

			std::clog << "Waiting for server input" << std::endl;
			// Получить количество соединений для текущего сокета с таймаутом
			int to_read = select(0, &readfds, nullptr, nullptr, &timeout);
			if (to_read == SOCKET_ERROR) {
				// Если сокет закрыт извне, можно выйти из цикла
				if (WSAGetLastError() == 10038) {
					outer_close = true;
					break;
				}

				std::cerr << getErrorTextWithWSAErrorCode("Couldn't select for socket") << std::endl;
			}
			else if (to_read > 0) {
				std::clog << "A server input received" << std::endl;
				int bytes_read;
				buffer.resize(TCP_MAX_MESSAGE_SIZE);
				// Если соединения есть, получаем его и запускаем поток для обработки
				if ((bytes_read = recv(
					this->socket_descriptor,
					&buffer[0],
					buffer.size(),
					0
				)) == SOCKET_ERROR) {
					std::cerr << getErrorTextWithWSAErrorCode("Couldnt get answer from a server") << std::endl;
					// Связь разорвана, выйти из цикла
					break;
				}
				else if (bytes_read != 0) {
					// Получено сообщение, оповестить при помощи метода onMessage
					buffer.resize(bytes_read);
					this->onMessage(buffer);
				}
				else break;
			}
		}

		std::clog << "Closing client socket" << std::endl;
		// Закрываем сокет
		if (!outer_close && closesocket(this->socket_descriptor) == SOCKET_ERROR)
			std::cerr << getErrorTextWithWSAErrorCode("Unable to close client socket");
	}
	catch (...) {
		std::cerr << "An error occured while handling server connection" << std::endl;
	}

	// Оповещаем при помощи метода onDisconnect о разрыве соединения
	this->onDisconnect();

	// Устанавливаем флаг работы в false
	*this->running = true;
}

void TCPClient::disconnect() {
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't disconnect from server"));
}

void TCPClient::request() {
	return IClient::request();
}