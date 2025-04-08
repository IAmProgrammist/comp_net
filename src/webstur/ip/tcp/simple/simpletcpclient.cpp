#include "pch.h"
#include <iostream>
#include <sstream>
#include <webstur/ip/tcp/simple/simpletcpclient.h>

SimpleTCPClient::SimpleTCPClient(std::string address, int port) : TCPClient(address, port) {}

void SimpleTCPClient::onMessage(const std::vector<char>& message) {
	std::cout << "A message was received! It says: " << std::string(message.begin(), message.end()) << std::endl;
	auto response = std::istringstream(std::string("Hey there, I can respond with TCP messages!\r\n\r\n"));
	this->sendMessage(response);
}

void SimpleTCPClient::onConnect() {
	std::cout << "Connection established with a new server in simple tcp client!" << std::endl;
}

void SimpleTCPClient::onDisconnect() {
	std::cout << "Connection stopped with a server in simple tcp client. Bye-bye!" << std::endl;
}