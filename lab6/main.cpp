#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <webstur/dhcp.h>

int main() {
	try {
		DHCPHelper::init();
		std::string input;

		// Считытваем команду пользователя
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
				// Ввести имя домена
				std::cout << "Enter domain name: ";
				std::cout.flush();
				std::string name;
				std::getline(std::cin, name);

				// Получить IP адрес
				auto result = DHCPHelper::getAddrInfo(name);

				char buf[64];
				InetNtopA(AF_INET, &((sockaddr_in *) result->ai_addr)->sin_addr, buf, sizeof(buf));

				std::cout << "IP address: " << std::string(buf, buf + strlen(buf)) << std::endl;
			}
			else if (input == "A") {
				// Ввести IP адрес
				sockaddr_in info;
				info.sin_family = AF_INET;
				std::cout << "Enter IP address: ";
				std::cout.flush();
				std::string name;
				std::getline(std::cin, name);

				char buf[64] = {};
				memcpy(buf, name.c_str(), name.size());
				InetPtonA(AF_INET, buf, &info.sin_addr);

				std::cout << "Name: " << DHCPHelper::getNameInfo(info) << std::endl;
			}
			else if (input == "D") {
				// Вывести информацию о сервере
				DHCPHelper::printDHCPServerInfo(std::cout << "DHCP address: ") << std::endl;
			}
			else if (input == "S") {
				// Обновить информацию об IP
				DHCPHelper::renewIP();
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

	// Выгрузка библиотеки WSA
	DHCPHelper::detach();

	return 0;
}