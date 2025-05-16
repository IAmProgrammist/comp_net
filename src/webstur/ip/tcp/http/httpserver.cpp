#include "pch.h"

#include <iostream>
#include <webstur/ip/tcp/http/httpserver.h>

HTTPServer::HTTPServer(int port) : TCPServer(port) {};

void HTTPServer::injectEndpoint(const HTTPEndpoint& endpoint) {
	this->endpoints.push_back(endpoint);
}

void HTTPServer::onConnect(SOCKET client) {
	std::clog << "A new HTTP server connection established" << std::endl;
}

void HTTPServer::onDisconnect(SOCKET client) {
	std::clog << "An HTTP connection is ended" << std::endl;
}

void HTTPServer::onMessage(SOCKET client, const std::vector<char>& message) {

}