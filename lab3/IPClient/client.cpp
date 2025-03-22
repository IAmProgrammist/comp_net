#include <iostream>
#include <WinSock2.h>
#include <cstdio>
#include <fstream>
#include "client.h"
#include "../shared.h"

Client::Client() {
	std::clog << "Creating socket" << std::endl;

	// Создаём сокет
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_IP
	);

	std::clog << "Making socket broadcast" << std::endl;
	// Делаем сокет способным к широковещательному каналу
	int timeout_ms = 10000;
	if (setsockopt(this->socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_ms, sizeof(timeout_ms)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to make socket broadcast"));

	std::clog << "Creating socket bind addr" << std::endl;
	// Создаём адрес к которому привяжется сокет
	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr = getDeviceAddrInfo().sin_addr;
	bind_addr.sin_port = htons(CLIENT_DEFAULT_PORT);

	std::clog << "Binding socket" << std::endl;
	// Привязать сокет к адресу
	if (bind(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to bind socket descriptor"));
}

Client::~Client() {
	std::clog << "Closing socket" << std::endl;
	// Закрываем сокет
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));
}

void Client::request(char* payload, int payload_size) {
	this->temporary_data.clear();
	std::clog << "Started accepting a file";
	char buffer[IMAGE_FRAGMENT_SIZE];
	int bytes_received;

	std::ofstream of(getUniqueFilepath().c_str());

	// Получаем сегменты и сохраняем их в буфер
	while ((bytes_received = recvfrom(
		this->socket_descriptor,
		buffer,
		sizeof(buffer),
		0,
		nullptr,
		nullptr)) != SOCKET_ERROR) {
		of.write(buffer, bytes_received);
	}

	of.flush();
	of.close();
	
	std::clog << "File accepted";
}

std::string Client::save() {
	return "";
}