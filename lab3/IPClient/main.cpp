#include <iostream>
#include <WinSock2.h>
#include <algorithm>
#include <webstur/utils.h>
#include <webstur/ip/udpclient.h>

int main() {
	try {
		// Инициализация библиотеки WSA
		IClient::init();

		// Создать клиент
		IClient* c = new UDPClient();

		// Оставляем клиент работать, пока пользователь не решит его приостановить
		std::string input;
		while (true)
		{
			std::cout << "Enter Y to request a file\n"
				<< "Enter N to stop client\n"
				<< "Enter L to save receive result into file\n"
				<< "Enter F to print received message info\n"
				<< "Enter E to exit loop\n"
				<< "Enter S to print status" << std::endl;

			std::cin >> input;
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "Y") {
				// Запустить клиент
				c->request();
			}
			else if (input == "N") {
				// Приостановить клиент
				c->shutdown();
			}
			else if (input == "E") {
				// Выход из цикла
				break;
			}
			else if (input == "L") {
				// Сохранить файл если он был успешно получен
				if (!c->isReady())
					std::cout << "Client response is not ready yet" << std::endl;

				std::string save_path = getUniqueFilepath();
				saveByteArray(c->getAnswer(), save_path);
				std::cout << "A response was saved at '" << save_path << "'" << std::endl;
			}
			else if (input == "F") {
				// Вывести информацию о файле если он был успешно получен
				if (!c->isReady())
					std::cout << "Client response is not ready yet" << std::endl;

				c->printAnswerInfo(std::cout);
			}
			else if (input == "S") {
				// Вывести информацию о клиенте
				c->printClientInfo(std::cout);
			}
		}

		// Приостановить сервер
		c->shutdown();

		delete c;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// Выгрузка библиотеки WSA
	IClient::detach();


	return 0;
}