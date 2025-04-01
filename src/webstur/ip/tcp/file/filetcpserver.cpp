#include "pch.h"
#include <iostream>
#include <fstream>
#include <webstur/ip/tcp/file/filetcpserver.h>

void FileTCPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::string file_path = std::string(message.begin(), message.end());
	
	std::clog << "A file at '" << file_path << "' is requested" << std::endl;

	// Открыть файл
	std::ifstream read_file(file_path);
	this->sendMessage(client, read_file);	
	this->disconnect(client);
}

void FileTCPServer::onConnect(SOCKET client) {
}

void FileTCPServer::onDisconnect(SOCKET client) {
}