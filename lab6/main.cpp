#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <webstur/dhcp.h>

int main() {
	try {
		DHCPHelper::init();
		std::string input;

		// ���������� ������� ������������
		while (true)
		{
			std::cout 
				<< "Enter T to get IP adress from domain name\n"
				<< "Enter A to get domain name from IP address\n"
				<< "Enter D to print IP address of DHCP server\n"
				<< "Enter S to renew IP address\n"
				<< "Enter E to exit loop"
				<< std::endl;

			std::cin >> input;
			std::cin.get();
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "T") {
				// ������ ��� ������
				std::cout << "Enter domain name: ";
				std::cout.flush();
				std::string name;
				std::getline(std::cin, name);

				// �������� IP �����
				auto result = DHCPHelper::getAddrInfo(name);

				char buf[64];
				InetNtopA(AF_INET, &((sockaddr_in *) result->ai_addr)->sin_addr, buf, sizeof(buf));

				std::cout << "IP address: " << std::string(buf, buf + strlen(buf)) << std::endl;
			}
			else if (input == "A") {
				// ������ IP �����
				sockaddr_in info;
				info.sin_family = AF_INET;
				std::cout << "Enter IP address: ";
				std::cout.flush();
				std::string name;
				std::getline(std::cin, name);

				char buf[64] = {};
				memcpy(buf, name.c_str(), name.size());
				InetPtonA(AF_INET, buf, &info.sin_addr);

				DHCPHelper::getNameInfo(info);
			}
			else if (input == "D") {
				// ������� ���������� � �������
				DHCPHelper::printDHCPServerInfo(std::cout);
			}
			else if (input == "S") {
				// �������� ���������� �� IP
				DHCPHelper::renewIP();
			}
			else if (input == "E") {
				// ����� �� �����
				break;
			}
		}
	}
	catch (const std::runtime_error& error) {
		std::cerr << "Failed while running server. Caused by: '" << error.what() << "'" << std::endl;

		return -1;
	}

	// �������� ���������� WSA
	DHCPHelper::detach();

	return 0;
}