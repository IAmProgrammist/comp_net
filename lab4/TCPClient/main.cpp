#include <iostream>
#include <webstur/ip/tcp/simple/simpletcpclient.h>

int main() {
	IClient::init();
	TCPClient* client = new SimpleTCPClient(std::string("192.168.1.175"), 23);
	client->start();
	client->printClientInfo(std::cout);
	while (true) {
	}
	delete client;
	IClient::detach();

	return 0;
}