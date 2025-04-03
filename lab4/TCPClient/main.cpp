#include <iostream>
#include <webstur/ip/tcp/file/filetcpclient.h>

int main() {
	IClient::init();
	TCPClient* client = new FileTCPClient(std::string("192.168.1.175"), 3178);
	client->request((char *)"C:\\Users\\vladi\\Downloads\\big_file.iso", 38);
	while (true) {
	}
	delete client;
	IClient::detach();

	return 0;
}