#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <cstdio>
#include <fstream>
#include <chrono>
#include <webstur/ip/udp/udpclient.h>
#include <webstur/utils.h>

UDPClient::UDPClient() {
	this->should_run = new std::atomic<bool>(false);
	this->running = new std::atomic<bool>(false);
	this->temporary_data = new std::vector<char>();

	std::clog << "Creating socket" << std::endl;
	// Создаём сокет
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_IP
	);

	std::clog << "Applying timeout for socket" << std::endl; 
	// Добавляем сокету таймаут
	int timeout_ms = 10000;
	if (setsockopt(this->socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_ms, sizeof(timeout_ms)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to make socket broadcast"));

	std::clog << "Creating socket bind addr" << std::endl;
	// Создаём адрес к которому привяжется сокет
	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr = getDeviceAddrInfo().sin_addr;
	bind_addr.sin_port = htons(UDP_CLIENT_DEFAULT_PORT);

	std::clog << "Binding socket" << std::endl;
	// Привязать сокет к адресу
	if (bind(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to bind socket descriptor"));
}

UDPClient::~UDPClient() {
	std::clog << "Stopping worker thread" << std::endl;
	// Приостанавливаем рабочий поток
	this->shutdown();

	std::clog << "Closing socket" << std::endl;
	// Закрываем сокет
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));

	delete this->should_run;
	delete this->running;
	delete this->temporary_data;
}

void UDPClient::request(char* payload, int payload_size) {
	// Если клиент уже работает, выходим из него
	if (*this->running) {
		std::clog << "UDPClient is already running" << std::endl;
		return;
	}

	std::clog << "Starting client" << std::endl;
	// Подготавливаем рабочий поток
	delete this->current_runner;
	*this->should_run = true;
	this->temporary_data->clear();
	this->current_runner = new std::thread([this]() {
		// Устанавливаем флаг работы потока на true
		*this->running = true;

		char buffer[UDP_IMAGE_FRAGMENT_SIZE];
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
			this->temporary_data->insert(this->temporary_data->end(), buffer, buffer + sizeof(buffer));
		}

		auto b = std::chrono::high_resolution_clock::now();

		std::clog << "Answer accepted\n" <<
			"Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() / 1000.0 << " s." << std::endl;

		// Устанавливаем флаг работы потока на false
		*this->running = false;
	});

	this->current_runner->detach();
}

bool UDPClient::isReady() {
	return !(*this->running);
}

const std::vector<char>& UDPClient::getAnswer() {
	return *this->temporary_data;
}

void UDPClient::wait_for_client_stop() {
	while (*this->running) {
	}
}

void UDPClient::shutdown() {
	if (!(*this->running)) {
		std::clog << "UDPClient is already stopped" << std::endl;
		return;
	}

	std::clog << "Stopping client" << std::endl;
	// Указываем что клиенту нужно приостановиться
	// и ждём пока он остановится
	*this->should_run = false;
	wait_for_client_stop();
	delete this->current_runner;
}

std::ostream& UDPClient::printClientInfo(std::ostream& out) {
	sockaddr_in client_address;
	int client_address_size = sizeof(client_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&client_address, &client_address_size);

	out << "IP client info:\n" <<
		"UDPClient state: " << (*this->running ? "running" : "not running") << "\n";

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

std::ostream& UDPClient::printAnswerInfo(std::ostream& out) {
	out << "Answer info:\n" << "  Size:\n";
	out << "    " << this->temporary_data->size() << " B\n";
	out << "    " << this->temporary_data->size() / 1024.0 << " KiB\n";
	out << "    " << this->temporary_data->size() / 1024.0 / 1024.0 << " MiB\n";

	out.flush();

	return out;
}

void UDPClient::request() {
	return IClient::request();
}