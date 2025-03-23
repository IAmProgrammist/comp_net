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
	this->should_run = false;

	std::clog << "Stopping worker thread" << std::endl;
	wait_for_client_stop();

	delete this->current_runner;

	std::clog << "Closing socket" << std::endl;
	// Закрываем сокет
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));
}

void Client::request(char* payload, int payload_size) {
	// Если клиент уже работает, выходим из него
	if (this->running) {
		std::clog << "Client is already running" << std::endl;
		return;
	}

	// Подготавливаем рабочий поток
	delete this->current_runner;

	std::clog << "Starting client" << std::endl;
	this->should_run = true;
	this->temporary_data.clear();

	this->current_runner = new std::thread([this]() {
		// Устанавливаем флаг работы потока на true
		this->running = true;

		char buffer[IMAGE_FRAGMENT_SIZE];
		int bytes_received;

		auto a = std::chrono::high_resolution_clock::now();

		// Получаем сегменты и сохраняем их в буфер
		while (should_run && (bytes_received = recvfrom(
			this->socket_descriptor,
			buffer,
			sizeof(buffer),
			0,
			nullptr,
			nullptr)) != SOCKET_ERROR) {
			this->temporary_data.insert(this->temporary_data.end(), buffer, buffer + sizeof(buffer));
		}

		auto b = std::chrono::high_resolution_clock::now();

		std::clog << "Answer accepted\n" <<
			"\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() / 1000.0 << " s." << std::endl;

		// Устанавливаем флаг работы потока на false
		this->running = false;
	});

	this->current_runner->detach();
}

bool Client::isReady() {
	return !this->running;
}

const std::vector<char>& Client::getAnswer() {
	return this->temporary_data;
}

void Client::wait_for_client_stop() {
	while (this->running) {
	}
}

void Client::shutdown() {
	if (!this->running) {
		std::clog << "Client is already stopped" << std::endl;
		return;
	}

	// Указываем что клиенту нужно приостановиться
	// и ждём пока он остановится
	std::clog << "Stopping client" << std::endl;
	this->should_run = false;

	wait_for_client_stop();

	delete this->current_runner;
}

// Отобразить информацию о клиенте
std::ostream& Client::printClientInfo(std::ostream& out) {
	sockaddr_in client_address;
	int client_address_size = sizeof(client_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&client_address, &client_address_size);

	std::cout << "Welcome to IP client!\n" <<
		"Client state: " << (this->running ? "running" : "not running") << "\n";

	if (get_sock_name_res == SOCKET_ERROR) {
		std::cout << "Unable to get socket info\n";
	}
	else {
		std::cout << "Socket info:\n";
		printSockaddrInfo(std::cout, client_address);
		std::cout << "\n";
	}

	std::cout.flush();

	return out;
}