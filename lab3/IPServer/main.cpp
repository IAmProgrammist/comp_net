#include <iostream>
#include <algorithm>
#include "server.h"

int main() {
	try {
		// Инициализация библиотеки WSA
		IServer::init();

		// Ввести путь к раздаваемому файлу
		std::cout << "Input path to file to send: ";
		std::cout.flush();
		std::string input;
		std::cin >> input;

		// Создать сервер
		IServer* s = new Server(input);

		// Оставляем сервер работать, пока пользователь не решит его приостановить
		while (true)
		{
			std::cout << "Enter Y to start broadcast send\n"
				<< "Enter N to stop server\n"
				<< "Enter E to exit loop\n"
				<< "Enter S to print status" << std::endl;

			std::cin >> input;
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "Y") {
				// Запустить сервер
				s->start();
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
				s->printServerInfo(std::cout);
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
	IServer::detach();


	return 0;
}