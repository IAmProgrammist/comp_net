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

TCPClient::TCPClient(std::string address, int port) {
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
	this->current_runner = new std::thread(TCPClient::connection, this);
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
	*this->running = true;
	std::clog << "Starting TCP client" << std::endl;
	// Запуск TCP-сервера
	try {

		sockaddr_in bind_addr;
		inet_pton(AF_INET, &this->server_address[0], &bind_addr);
		bind_addr.sin_family = AF_INET;
		bind_addr.sin_port = htons(this->server_port);

		// Подключаемся к серверу
		if (connect(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
			throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't connect to server at " +
				this->server_address + ":" + std::to_string(this->server_port)));

		// Закрываем сокет
		if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
			throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't connect to server at "));
	}
	catch (std::runtime_error& er) {
		std::cerr << er.what() << std::endl;
	}

	closesocket(this->socket_descriptor);
	*this->running = true;
}

void TCPClient::request() {
	return IClient::request();
}