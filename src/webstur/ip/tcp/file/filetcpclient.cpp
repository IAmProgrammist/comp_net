#include "pch.h"
#include <iostream>
#include <webstur/ip/tcp/file/filetcpclient.h>

FileTCPClient::FileTCPClient(std::string address, int port, std::string save_path): 
	TCPClient(address, port), save_path(save_path) {
}

void FileTCPClient::onConnect() {
	std::clog << "A file server connection established, opening a file to write" << std::endl;

	// Открываем файл для записи
	this->save_file = std::ofstream(save_path, std::ios::binary);

	if (!this->save_file.good()) {
		this->shutdown();
		throw std::invalid_argument("Couldn't open file for saving at '" + save_path + "'");
	}
	std::clog << "Saving response at '" << this->save_path << "'" << std::endl;
}

void FileTCPClient::onDisconnect() {
	// Сохраняем файл
	this->save_file.flush();
	this->save_file.close();
}

void FileTCPClient::onMessage(const std::vector<char>& message) {
	// Пишем сообщение в файл
	this->save_file.write(&message[0], message.size());
}