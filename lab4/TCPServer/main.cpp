#include <iostream>
#include <webstur/ip/simpletcpserver.h>

int main() {
	IServer::init();
	TCPServer* server = new SimpleTCPServer();
	server->start();
	server->printServerInfo(std::cout);
	while (true) {
	}
	delete server;
	IServer::detach();

	return 0;
}