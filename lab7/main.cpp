#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <fstream>
#include "mypop3client.h"
#include "mysmtpclient.h"

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
		MySMTPClient smtp_client(address);
		smtp_client.setLogin(login);
		std::ifstream file_to_send;
		smtp_client.setDataStream(file_to_send);

		// Считытваем команду пользователя
		while (true)
		{
			std::cout 
				<< "Enter 1 to get mail list\n"
				<< "Enter 2 to retrieve mail\n"
				<< "Enter 3 to delete mail\n"
				<< "Enter 4 to send mail\n"
				<< "Enter 5 to exit program\n"
				<< std::endl;

			std::string input;
			std::cin >> input;
			std::cin.get();
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "1") {
				// Отправить запрос на список писем
				POP3Request mail_list_request;
				mail_list_request.task = POP3Tasks::LIST;
				client.request((const char*)&mail_list_request, sizeof(mail_list_request));
			}
			else if (input == "2") {
				// Отправить запрос на письмо
				POP3Request mail_request;
				mail_request.task = POP3Tasks::MAIL;
				// Ввести ID письма
				std::cout << "Input mail ID to retrieve: " << std::flush;
				std::cin >> mail_request.arguments.id;
				client.request((const char*)&mail_request, sizeof(mail_request));
			}
			else if (input == "3") {
				// Отправить запрос на удаление письма
				POP3Request mail_request;
				mail_request.task = POP3Tasks::DEL;
				// Ввести ID письма
				std::cout << "Input mail ID to delete: " << std::flush;
				std::cin >> mail_request.arguments.id;
				client.request((const char*)&mail_request, sizeof(mail_request));
			}
			else if (input == "4") {
				// Ввести получателя и файл для отправки
				std::string recipient;
				std::cout << "Input recipient: " << std::flush;
				std::cin >> recipient;
				std::string file_path;
				std::cout << "Input file path: " << std::flush;
				std::cin >> file_path;

				smtp_client.setReceiver(recipient);
				if (file_to_send.is_open())
					file_to_send.close();
				file_to_send.open(file_path);

				smtp_client.start();
			}
			else if (input == "5") {
				// Отправить запрос на закрытие клиента
				POP3Request quit_request;
				quit_request.task = POP3Tasks::QUIT;
				client.request((const char*)&quit_request, sizeof(quit_request));

				break;
			}
		}
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}
	IClient::detach();

	return 0;
}