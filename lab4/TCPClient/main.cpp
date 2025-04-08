#include <iostream>
#include <algorithm>
#include <webstur/ip/tcp/file/filetcpclient.h>

int main() {
	try {
		// Инициализация библиотеки WSA
		IClient::init();

		// Ввести ip и порт
		std::cout << "Enter IP Address and port" << std::endl;
		std::string address;
		int port;
		std::cin >> address >> port;

		// Создать клиент
		TCPClient* c = new FileTCPClient(std::string(address), port);

		// Оставляем клиент работать, пока пользователь не решит его приостановить
		std::string input;
		while (true)
		{
			std::cout << "Enter Y to request a file\n"
				<< "Enter N to stop client\n"
				<< "Enter E to exit loop\n"
				<< "Enter S to print status" << std::endl;

			std::cin >> input;
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "Y") {
				// Ввести запрашиваемый файл
				std::cout << "Enter file to request: ";
				std::cout.flush();
				std::cin >> input;
				// Запустить клиент
				c->request(&input[0], input.size());
			}
			else if (input == "N") {
				// Приостановить клиент
				c->shutdown();
			}
			else if (input == "E") {
				// Выход из цикла
				break;
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
		std::cerr << "Failed while running client. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// Выгрузка библиотеки WSA
	IClient::detach();

	return 0;
}