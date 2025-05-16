#include <iostream>
#include <fstream>
#include <filesystem>
#include "filetcpserver.h"

void FileTCPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::string file_path = std::string(message.begin(), message.end());
	
	std::clog << "A file at '" << file_path << "' is requested" << std::endl;

	// Открыть файл
	std::ifstream read_file(file_path, std::ios::binary|std::ios::ate);
	auto total_bytes = read_file.tellg();
	read_file.seekg(0, std::ios::beg);
	auto start_time = std::chrono::high_resolution_clock::now();
	// Отправить файл
	this->sendMessage(client, read_file);
	auto end_time = std::chrono::high_resolution_clock::now();
	std::clog << "A message sent to a client\n" <<
		"  Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0 << " seconds.\n" <<
		"  Size:\n" <<
		"    " << total_bytes << " B\n" <<
		"    " << total_bytes / 1024.0 << " KiB\n" <<
		"    " << total_bytes / 1024.0 / 1024.0 << " MiB\n" << std::endl;
	// Закрыть файл
	read_file.close();
	// Закрыть соединение
	this->disconnect(client);
}

void FileTCPServer::onConnect(SOCKET client) {
	std::clog << "A new user connection is established" << std::endl;
}

void FileTCPServer::onDisconnect(SOCKET client) {
	std::clog << "User disconnected" << std::endl;
}