#include "pch.h"
#include <iostream>
#include <webstur/ip/tcp/simple/simpletcpserver.h>

void SimpleTCPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::cout << "A message was received! It says: " << std::string(message.begin(), message.end()) << std::endl;
	this->disconnect(client);
}

void SimpleTCPServer::onConnect(SOCKET client) {
	std::cout << "Connection established with a new client in simple tcp server!" << std::endl;
}

void SimpleTCPServer::onDisconnect(SOCKET client) {
	std::cout << "Connection stopped with a client in simple tcp server. Bye-bye!" << std::endl;
}