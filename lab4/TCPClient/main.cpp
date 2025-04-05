#include <iostream>
#include <algorithm>
#include <webstur/ip/tcp/file/filetcpclient.h>

int main() {
	try {
		// ������������� ���������� WSA
		IClient::init();

		// ������ ip � ����
		std::cout << "Enter IP Address and port" << std::endl;
		std::string address;
		int port;
		std::cin >> address >> port;

		// ������� ������
		TCPClient* c = new FileTCPClient(std::string(address), port);

		// ��������� ������ ��������, ���� ������������ �� ����� ��� �������������
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
				// ������ ������������� ����
				std::cout << "Enter file to request: ";
				std::cout.flush();
				std::cin >> input;
				// ��������� ������
				c->request(&input[0], input.size());
			}
			else if (input == "N") {
				// ������������� ������
				c->shutdown();
			}
			else if (input == "E") {
				// ����� �� �����
				break;
			}
			else if (input == "S") {
				// ������� ���������� � �������
				c->printClientInfo(std::cout);
			}
		}

		// ������������� ������
		c->shutdown();

		delete c;
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running client. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// �������� ���������� WSA
	IClient::detach();

	return 0;
}