#include <iostream>
#include "server.h"

int main() {
	try {
		// ������������� ���������� WSA
		IServer::init();

		// ������ ���� � ������������ �����
		std::cout << "Input path to file to send: ";
		std::cout.flush();
		std::string input;
		std::cin >> input;

		// ������� ������
		IServer* s = new Server(input);

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
				s->start();
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
				s->printServerInfo(std::cout);
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
	IServer::detach();


	return 0;
}