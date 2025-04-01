#include "pch.h"
#include <istream>
#include <iostream>
#include <webstur/ip/tcp/tcpclient.h>

void TCPClient::sendMessage(SOCKET server, std::istream& message) {
	char buffer[TCP_MAX_MESSAGE_SIZE];

	// Отправить данные клиенту
	while (!message.eof()) {
		auto bytes_read = message.readsome(buffer, sizeof(buffer));

		if (bytes_read == 0) break;

		if (send(server, buffer, bytes_read, 0) == SOCKET_ERROR)
			throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't send data over TCP"));
	}
}

void TCPClient::disconnect(SOCKET server) {
	if (closesocket(server) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Couldn't close connection for socket"));
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

}

void TCPClient::start() {

}

void TCPClient::shutdown() {

}

std::ostream& TCPClient::printClientInfo(std::ostream& out) {

}

void TCPClient::waitForClientStop() {

}