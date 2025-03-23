#include <iostream>
#include <WinSock2.h>
#include "client.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	try {
		// Инициализация библиотеки WSA
		IClient::init();

		// Ввести путь к раздаваемому файлу
		std::cout << "Input path to file to send: ";
		std::cout.flush();
		std::string input;
		std::cin >> input;

		// Создать сервер
		IClient* s = new Client();

		// Оставляем сервер работать, пока пользователь не решит его приостановить
		while (true)
		{
			std::cout << "Enter Y to start broadcast send\n"
				<< "Enter N to stop server\n"
				<< "Enter E to exit loop\n"
				<< "Enter S to print status" << std::endl;

			std::cin >> input;
			if (input == "Y") {
				// Запустить сервер
				//s->start();
			}
			else if (input == "N") {
				// Приостановить сервер
				s->shutdown();
			}
			else if (input == "E") {
				// Выход из цикла
				break;
			}
			else if (input == "S") {
				// Вывести информацию о сервере
				//s->printServerInfo(std::cout);
			}
		}

		// Приостановить сервер
		s->shutdown();

		delete s;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// Выгрузка библиотеки WSA
	IClient::detach();


	return 0;
}