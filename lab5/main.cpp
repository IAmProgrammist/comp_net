#include <iostream>
#include <algorithm>
#include <webstur/ip/arp.h>
#include <webstur/iserver.h>

int main() {
	setlocale(LC_ALL, "Russian");
	try {
		IServer::init();
		std::string input;
		
		// Считытваем команду пользователя
		while (true)
		{
			std::cout << "Введите T чтобы вывести ARP таблицу\n"
				      << "Введите A чтобы добавить запись в ARP таблицу\n"
				      << "Введите D чтобы удалить запись из ARP таблицы\n"
				      << "Введите S чтобы отправить ARP запрос\n"
				      << "Введите E чтобы закончить" << std::endl;

			std::cin >> input;
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "T") {
				// Вывести ARP таблицу
				ARPHelper::printARPTable(std::cout);
			}
			else if (input == "A") {
				// Ввести IP адрес, физический адрес и индекс интерфейса
				std::string ip_address, physical_address;
				int interface_index;
				std::cout << "IP адрес: ";
				std::cout.flush();
				std::cin >> ip_address;
				std::cout << "MAC адрес: ";
				std::cout.flush();
				std::cin >> physical_address;
				std::cout << "Индекс интерфейса: ";
				std::cout.flush();
				std::cin >> interface_index;

				// Добавить запись
				ARPHelper::addArpEntry(ip_address, physical_address, interface_index);
			}
			else if (input == "D") {
				// Ввести IP адрес, физический адрес и индекс интерфейса
				std::string ip_address, physical_address;
				int interface_index;
				std::cout << "IP адрес: ";
				std::cout.flush();
				std::cin >> ip_address;
				std::cout << "MAC адрес: ";
				std::cout.flush();
				std::cin >> physical_address;
				std::cout << "Индекс интерфейса: ";
				std::cout.flush();
				std::cin >> interface_index;

				// Удалить запись
				ARPHelper::deleteArpEntry(ip_address, physical_address, interface_index);
			}
			else if (input == "S") {
				// Ввести IP адрес
				std::string ip_address;
				std::cout << "IP адрес: ";
				std::cout.flush();
				std::cin >> ip_address;

				// Отправить ARP запрос
				std::cout << "Полученный MAC-адрес: " << ARPHelper::sendARP(ip_address) << std::endl;
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
	IServer::detach();


	return 0;
}