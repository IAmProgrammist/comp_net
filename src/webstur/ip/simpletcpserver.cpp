#include "pch.h"
#include <iostream>
#include <webstur/ip/simpletcpserver.h>

void SimpleTCPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::cout << "A message was received! It says: " << std::string(message.begin(), message.end()) << std::endl;
}