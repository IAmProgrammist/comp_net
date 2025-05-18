#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <webstur/ip/tcp/http/httpserver.h>
#include "endpoints/cardendpoint.h"

int main() {
	try {
		// Загрузить библиотеки WSA
		IServer::init();

		// Инициализировать сервер и эндпоинты
		CardEndpoint endpoint;
		auto server = HTTPServer();
		server.injectEndpoint(endpoint);
		
		// Запустить сервер
		server.start();

		std::cout << "A server is running now. To exit server gracefully press ENTER key" << std::endl;
		// При любом вводе пользователя приостановить выполнение программы
		std::cin.ignore();
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// Выгрузка библиотеки WSA
	IServer::detach();

	return 0;
}