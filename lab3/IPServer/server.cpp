#include <exception>
#include <WinSock2.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include "server.h"
#include "../shared.h"

#pragma comment(lib, "ws2_32.lib")

Server::Server(std::string file_path, int port) {
	std::clog << "Opening file " << file_path << std::endl;
	// ��������� ����
	this->file = new std::ifstream(file_path, std::ios::binary);
	if (!this->file->is_open())
		throw std::runtime_error("Unable to open file for read " + file_path);

	std::clog << "Creating socket" << std::endl;

	// ������ �����
	this->socket_descriptor = socket(
		AF_INET,
		SOCK_DGRAM,
		IPPROTO_IP
	);

	std::clog << "Making socket broadcast" << std::endl;
	// ������ ����� ��������� � ������������������ ������
	bool broadcast = true;
	if (setsockopt(this->socket_descriptor, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to make socket broadcast"));

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
}

Server::~Server() {
	this->should_run = false;

	std::clog << "Stopping worker thread" << std::endl;
	wait_for_server_stop();

	delete this->current_runner;

	std::clog << "Closing file" << std::endl;
	// ��������� ����
	this->file->close();
	delete this->file;

	std::clog << "Closing socket" << std::endl;
	// ��������� �����
	if (closesocket(this->socket_descriptor) == SOCKET_ERROR)
		throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to close socket"));
}

std::ostream& Server::printServerInfo(std::ostream& out) {
	sockaddr_in server_address;
	int server_address_size = sizeof(server_address);
	int get_sock_name_res = getsockname(this->socket_descriptor, (sockaddr*)&server_address, &server_address_size);

	std::cout << "Welcome to IP server!\n" <<
		"Server state: " << (this->running ? "running" : "not running") << "\n";

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

void Server::start() {
	// ���� ������ ��� ��������, ������� �� ����
	if (this->running) {
		std::clog << "Server is already running" << std::endl;
		return;
	}

	// �������������� ������� �����
	delete this->current_runner;

	std::clog << "Starting server" << std::endl;
	this->should_run = true;

	this->current_runner = new std::thread([this]() {
		// ������������� ���� ������ ������ �� true
		this->running = true;

		this->file->clear();
		this->file->seekg(0, std::ios::beg);
		char buffer[IMAGE_FRAGMENT_SIZE];
		int packages_success = 0, packages_failed = 0;

		// ������������ ����������������� sockaddr_in
		sockaddr_in client_sockaddr;
		client_sockaddr.sin_family = AF_INET;
		client_sockaddr.sin_port = htons(CLIENT_DEFAULT_PORT);
		memset(&client_sockaddr.sin_addr, 0xff, 4);        
		
		auto a = std::chrono::high_resolution_clock::now();

		// ���� ����� ������ �������� � �� ��������� ����� �����
		while (this->should_run && !this->file->eof()) {
			// ������� ����
			this->file->read(buffer, sizeof(buffer));
			int bytes_read = this->file->gcount();

			// ��������� ��������
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

		std::clog << "A file broadcasted ended\nPackages failed: "
			<< packages_failed << "\nPackages sent: " << packages_success <<
			"\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() / 1000.0 << " s." << std::endl;

		// ������������� ���� ������ ������ �� false
		this->running = false;
	});

	this->current_runner->detach();
}

void Server::shutdown() {
	if (!this->running) {
		std::clog << "Server is already stopped" << std::endl;
		return;
	}

	// ��������� ��� ������� ����� ���������������
	// � ��� ���� �� �����������
	std::clog << "Stopping server" << std::endl;
	this->should_run = false;

	wait_for_server_stop();

	delete this->current_runner;
}

void Server::wait_for_server_stop() {
	// ���������� �������, ��� ��� ������ ������������ ��������� � ������ ������ 
	// ������ ������� ��� ���� ����������� ������
	while (this->running) {
	}
}