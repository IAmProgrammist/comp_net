#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include "mypop3client.h"

int main() {
	try {
		IClient::init();
		std::string address, login, password;

		std::cout << "Input server address: " << std::flush;
		std::cin >> address;
		std::cout << "Input login: " << std::flush;
		std::cin >> login;
		std::cout << "Input password: " << std::flush;
		std::cin >> password;

		MyPOP3Client client(address, login, password);
		client.start();

		// Считытваем команду пользователя
		while (true)
		{
			std::cout 
				<< "Enter 1 to get mail list\n"
				<< "Enter 2 to retrieve mail\n"
				<< "Enter 3 to delete mail\n"
				<< "Enter 4 to exit program\n"
				<< std::endl;

			std::string input;
			std::cin >> input;
			std::cin.get();
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "1") {
				// Отправить запрос на список писем
				POP3Request mail_list_request;
				mail_list_request.task = LIST;
				client.request((const char*)&mail_list_request, sizeof(mail_list_request));
			}
			else if (input == "2") {
				// Отправить запрос на письмо
				POP3Request mail_request;
				mail_request.task = MAIL;
				// Ввести ID письма
				std::cout << "Input mail ID to retrieve: " << std::flush;
				std::cin >> mail_request.arguments.id;
				client.request((const char*)&mail_request, sizeof(mail_request));
			}
			else if (input == "3") {
				// Отправить запрос на удаление письма
				POP3Request mail_request;
				mail_request.task = DEL;
				// Ввести ID письма
				std::cout << "Input mail ID to delete: " << std::flush;
				std::cin >> mail_request.arguments.id;
				client.request((const char*)&mail_request, sizeof(mail_request));
			}
			else if (input == "4") {
				// Отправить запрос на закрытие клиента
				POP3Request quit_request;
				quit_request.task = QUIT;
				client.request((const char*)&quit_request, sizeof(quit_request));
			}
			else if (input == "E") {
				// Выход из цикла
				break;
			}
		}
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	return 0;
}