#include "pch.h"
#include <iostream>
#include <webstur/ip/tcpserver.h>

TCPServer::TCPServer(int port) {
	this->should_run = new std::atomic<bool>(false);
	this->running = new std::atomic<bool>(false);

	std::clog << "Creating socket" << std::endl;

	// ������ �����
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP
	);

	std::clog << "Creating socket bind addr" << std::endl;
	// ������ ����� � �������� ���������� �����
	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr = getDeviceAddrInfo().sin_addr;
	bind_addr.sin_port = htons(port);

	std::clog << "Binding socket" << std::endl;
	// ��������� ����� � ������
	if (bind(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to bind socket descriptor"));

	std::clog << "Making socket to listen for connections" << std::endl;
	// ��������� ����� � ����� �������������
	if (listen(this->socket_descriptor, SOMAXCONN) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to make socket to listen"));
}

TCPServer::~TCPServer() {
	std::clog << "Stopping worker thread" << std::endl;
	// ���������������� ������� �����
	this->shutdown();

	std::clog << "Closing socket" << std::endl;
	// ��������� �����
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));

	delete this->should_run;
	delete this->running;
}

std::ostream& TCPServer::printServerInfo(std::ostream& out) {
	sockaddr_in server_address;
	int server_address_size = sizeof(server_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&server_address, &server_address_size);

	std::cout << "TCP server info:\n" <<
		"Server state: " << (*this->running ? "running" : "not running") << "\n";

	if (get_sock_name_res == SOCKET_ERROR) {
		std::cout << "Unable to get socket info\n";
	}
	else {
		std::cout << "Socket info:\n";
		printSockaddrInfo(std::cout, server_address);
		std::cout << "\n";
	}

	std::cout.flush();

	return out;
}

void TCPServer::start() {
	// ���� ������ ��� ��������, ������� �� ����
	if (*this->running) {
		std::clog << "Server is already running" << std::endl;
		return;
	}

	// �������������� ������� �����
	delete this->current_runner;
	delete this->current_threads_amount;

	std::clog << "Starting server" << std::endl;
	*this->should_run = true;
	this->current_threads_amount = new std::atomic<int>(0);
	this->current_runner = new std::thread(&TCPServer::serve, this);

	this->current_runner->detach();
}

void TCPServer::shutdown() {
	if (!(*this->running)) {
		std::clog << "Server is already stopped" << std::endl;
		return;
	}

	std::clog << "Stopping server" << std::endl;
	// ��������� ��� ������� ����� ���������������
	// � ��� ���� �� �����������
	*this->should_run = false;
	this->waitForServerStop();
	delete this->current_runner;
	delete this->current_threads_amount;
}

void TCPServer::waitForServerStop() {
	// ���������� �������, ��� ��� ������ ������������ ��������� � ������ ������ 
	// ������ ������� ��� ���� ����������� ������
	while (*this->running) {
	}
}

void TCPServer::serve() {
	std::clog << "Started TCPServer work" << std::endl;
	// ������������� ���� ������ ������ �� true
	*this->running = true;

	FD_SET readfds;
	timeval timeout;
	timeout.tv_sec = TCP_SERVER_TIMEOUT_S;
	timeout.tv_usec = 0;
	// ���� ������ ������ ��������
	while (*this->should_run) {
		FD_ZERO(&readfds);
		FD_SET(this->socket_descriptor, &readfds);

		std::clog << "Waiting for client connections" << std::endl;
		// �������� ���������� ���������� ��� �������� ������ � ���������
		int connections_amount = select(0, &readfds, nullptr, nullptr, &timeout);
		if (connections_amount == SOCKET_ERROR) {
			std::cerr << getErrorTextWithWSAErrorCode("Couldn't select for socket") << std::endl;
		}
		else if (connections_amount > 0) {
			std::clog << "Waiting for client connections" << std::endl;
			// ���� ���������� ����, �������� ��� � ��������� ����� ��� ���������
			SOCKET client_descriptor = accept(
				socket_descriptor,
				nullptr,
				nullptr);
			std::thread client_thread(&TCPServer::serveClient, this, client_descriptor);
			client_thread.detach();
		}
	}

	std::cout << "Waiting for children threads to stop" << std::endl;
	// ��� ���� �� �������� ������ ������ ��� ������ � ���������
	while (*this->current_threads_amount > 0) {
	}

	std::cout << "TCPServer stopped" << std::endl;
	// ������������� ���� ������ ������ �� false
	*this->running = false;
}

void TCPServer::serveClient(SOCKET client) {
	std::clog << "Serving client connection" << std::endl;
	// ����������� ���������� ������� �������
	(*this->current_threads_amount)++;

	try {
		std::vector<char> buffer(0, TCP_MAX_MESSAGE_SIZE);
		FD_SET readfds;
		timeval timeout;
		timeout.tv_sec = TCP_SERVER_TIMEOUT_S;
		timeout.tv_usec = 0;
		while (*this->should_run) {
			FD_ZERO(&readfds);
			FD_SET(client, &readfds);

			std::clog << "Waiting for client input" << std::endl;
			// �������� ���������� ���������� ��� �������� ������ � ���������
			int to_read = select(0, &readfds, nullptr, nullptr, &timeout);
			if (to_read == SOCKET_ERROR) {
				std::cerr << getErrorTextWithWSAErrorCode("Couldn't select for socket") << std::endl;
			}
			else if (to_read > 0) {
				std::clog << "Waiting for client connections" << std::endl;
				int bytes_read;
				buffer.resize(TCP_MAX_MESSAGE_SIZE);
				// ���� ���������� ����, �������� ��� � ��������� ����� ��� ���������
				if ((bytes_read = recv(
					client,
					&buffer[0],
					buffer.size(),
					0
				)) == SOCKET_ERROR) {
					std::cerr << getErrorTextWithWSAErrorCode("Couldnt get answer from a client") << std::endl;
					// ����� ���������, ����� �� �����
					break;
				}
				else if (bytes_read != 0) {
					// �������� ���������, ������������� ��� ������������
					buffer.resize(bytes_read);
					this->onMessage(client, buffer);
				}
				else break;
			}
		}

		std::clog << "Closing client socket" << std::endl;
		// ��������� �����
		if (closesocket(client) == SOCKET_ERROR)
			std::cerr << getErrorTextWithWSAErrorCode("Unable to close client socket");
	}
	catch (...) {
		std::cerr << "An error occured while handling user connection";
	}

	std::clog << "Exiting serve client thread" << std::endl;
	// ��������� ���������� ������� �������
	(*this->current_threads_amount)--;
}