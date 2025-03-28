#include "pch.h"
#include <exception>
#include <WinSock2.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <webstur/ip/udpserver.h>
#include <webstur/utils.h>

UDPServer::UDPServer(std::string file_path, int port) {
	this->should_run = new std::atomic<bool>(false);
	this->running = new std::atomic<bool>(false);

	std::clog << "Opening file " << file_path << std::endl;
	// Открываем файл
	this->file = new std::ifstream(file_path, std::ios::binary);
	if (!this->file->is_open())
		throw std::runtime_error("Unable to open file for read " + file_path);

	std::clog << "Creating socket" << std::endl;

	// Создаём сокет
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_IP
	);

	std::clog << "Making socket broadcast" << std::endl;
	// Делаем сокет способным к широковещательному каналу
	bool broadcast = true;
	if (setsockopt(this->socket_descriptor, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to make socket broadcast"));

	std::clog << "Creating socket bind addr" << std::endl;
	// Создаём адрес к которому привяжется сокет
	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr = getDeviceAddrInfo().sin_addr;
	bind_addr.sin_port = htons(port);

	std::clog << "Binding socket" << std::endl;
	// Привязать сокет к адресу
	if (bind(this->socket_descriptor, (sockaddr*)&bind_addr, sizeof(bind_addr)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to bind socket descriptor"));
}

UDPServer::~UDPServer() {
	std::clog << "Stopping worker thread" << std::endl;
	// Приостанавливаем рабочий поток
	this->shutdown();

	std::clog << "Closing file" << std::endl;
	// Закрываем файл
	this->file->close();
	delete this->file;

	std::clog << "Closing socket" << std::endl;
	// Закрываем сокет
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));

	delete this->should_run;
	delete this->running;
}

std::ostream& UDPServer::printServerInfo(std::ostream& out) {
	sockaddr_in server_address;
	int server_address_size = sizeof(server_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&server_address, &server_address_size);

	std::cout << "IP server info:\n" <<
		"UDPServer state: " << (*this->running ? "running" : "not running") << "\n";

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

void UDPServer::start() {
	// Если сервер уже работает, выходим из него
	if (*this->running) {
		std::clog << "UDPServer is already running" << std::endl;
		return;
	}

	// Подготавливаем рабочий поток
	delete this->current_runner;

	std::clog << "Starting server" << std::endl;
	*this->should_run = true;

	this->current_runner = new std::thread([this]() {
		// Устанавливаем флаг работы потока на true
		*this->running = true;

		this->file->clear();
		this->file->seekg(0, std::ios::beg);
		char buffer[UDP_IMAGE_FRAGMENT_SIZE];
		int packages_success = 0, packages_failed = 0;

		// Конструируем широковещательный sockaddr_in
		sockaddr_in client_sockaddr;
		client_sockaddr.sin_family = AF_INET;
		client_sockaddr.sin_port = htons(UDP_CLIENT_DEFAULT_PORT);
		client_sockaddr.sin_addr = getDeviceAddrInfo().sin_addr;
		memset(((char*)&client_sockaddr.sin_addr) + 3, 0xff, 1);

		int total_bytes = 0;
		
		auto a = std::chrono::high_resolution_clock::now();

		// Пока поток должен работать и не достигнут конец файла
		while (*this->should_run && !this->file->eof()) {
			// Считать файл
			this->file->read(buffer, sizeof(buffer));
			int bytes_read = static_cast<int>(this->file->gcount());
			total_bytes += bytes_read;

			// Отправить фрагмент
			if (sendto(
				this->socket_descriptor,
				buffer,
				bytes_read,
				0,
				(sockaddr*)&client_sockaddr,
				sizeof(client_sockaddr)) == SOCKET_ERROR) {
				packages_failed++;
			}
			else {
				packages_success++;
			}
		}

		auto b = std::chrono::high_resolution_clock::now();

		std::clog << "A file broadcasted ended\n  Packages failed: "
			<< packages_failed << "\n  Packages sent: " << packages_success << "\n" <<
			"  Size:" << "\n" <<
			"    " << total_bytes << " B\n" <<
			"    " << total_bytes / 1024.0 << " KiB\n" <<
			"    " << total_bytes / 1024.0 / 1024.0 << " MiB\n" <<
			"\n  Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() / 1000.0 << " s." << std::endl;

		// Устанавливаем флаг работы потока на false
		*this->running = false;
	});

	this->current_runner->detach();
}

void UDPServer::shutdown() {
	if (!(*this->running)) {
		std::clog << "UDPServer is already stopped" << std::endl;
		return;
	}

	std::clog << "Stopping server" << std::endl;
	// Указываем что серверу нужно приостановиться
	// и ждём пока он остановится
	*this->should_run = false;
	wait_for_server_stop();
	delete this->current_runner;
}

void UDPServer::wait_for_server_stop() {
	// Используем спинлок, так как пакеты относительно небольшие и сервер должен 
	// быстро увидеть что пора заканчивать работу
	while (*this->running) {
	}
}