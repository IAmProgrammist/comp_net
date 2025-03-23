#include <iostream>
#include <WinSock2.h>
#include "client.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
	try {
		// ������������� ���������� WSA
		IClient::init();

		// ������ ���� � ������������ �����
		std::cout << "Input path to file to send: ";
		std::cout.flush();
		std::string input;
		std::cin >> input;

		// ������� ������
		IClient* s = new Client();

		// ��������� ������ ��������, ���� ������������ �� ����� ��� �������������
		while (true)
		{
			std::cout << "Enter Y to start broadcast send\n"
				<< "Enter N to stop server\n"
				<< "Enter E to exit loop\n"
				<< "Enter S to print status" << std::endl;

			std::cin >> input;
			if (input == "Y") {
				// ��������� ������
				//s->start();
			}
			else if (input == "N") {
				// ������������� ������
				s->shutdown();
			}
			else if (input == "E") {
				// ����� �� �����
				break;
			}
			else if (input == "S") {
				// ������� ���������� � �������
				//s->printServerInfo(std::cout);
			}
		}

		// ������������� ������
		s->shutdown();

		delete s;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// �������� ���������� WSA
	IClient::detach();


	return 0;
}