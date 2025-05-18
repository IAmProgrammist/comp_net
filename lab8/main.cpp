#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <webstur/ip/tcp/http/httpserver.h>

int main() {
	try {
		IServer::init();

		auto server = HTTPServer(8081);
		server.start();

		while (1) {}
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// Выгрузка библиотеки WSA
	IServer::detach();

	return 0;
}