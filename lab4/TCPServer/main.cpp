#include <iostream>
#include <webstur/ip/tcp/file/filetcpserver.h>

int main() {
	IServer::init();
	TCPServer* server = new FileTCPServer();
	server->start();
	server->printServerInfo(std::cout);
	while (true) {
	}
	delete server;
	IServer::detach();

	return 0;
}