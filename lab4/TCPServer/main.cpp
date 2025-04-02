#include <iostream>
#include <fstream>
#include <webstur/ip/tcp/file/filetcpserver.h>

int main() {
	std::ifstream read_file("C:/LoremIpsum.txt", std::ios::binary);
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