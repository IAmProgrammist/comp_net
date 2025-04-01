#include "pch.h"
#include <iostream>
#include <fstream>
#include <webstur/ip/tcp/file/filetcpserver.h>

void FileTCPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::string file_path = std::string(message.begin(), message.end());
	
	std::clog << "A file at '" << file_path << "' is requested" << std::endl;

	// ������� ����
	std::ifstream read_file(file_path);
	if (!read_file.good()) {
		// ���� �� ������� �������, ��������� ����������
		std::cerr << "Invalid path specified: '" << file_path << "'" << std::endl;
		this->disconnect(client);
	}

	// ���� �� ��������� ����� �����
	while (!read_file.eof()) {

	}
}

void FileTCPServer::onConnect(SOCKET client) {
}

void FileTCPServer::onDisconnect(SOCKET client) {
}