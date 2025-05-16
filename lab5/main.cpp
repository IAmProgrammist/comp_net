#include <iostream>
#include <algorithm>
#include <webstur/ip/arp.h>
#include <webstur/iserver.h>

int main() {
	setlocale(LC_ALL, "Russian");
	try {
		IServer::init();
		std::string input;
		
		// ���������� ������� ������������
		while (true)
		{
			std::cout << "������� T ����� ������� ARP �������\n"
				      << "������� A ����� �������� ������ � ARP �������\n"
				      << "������� D ����� ������� ������ �� ARP �������\n"
				      << "������� S ����� ��������� ARP ������\n"
				      << "������� E ����� ���������" << std::endl;

			std::cin >> input;
			std::transform(input.begin(), input.end(), input.begin(), toupper);
			if (input == "T") {
				// ������� ARP �������
				ARPHelper::printARPTable(std::cout);
			}
			else if (input == "A") {
				// ������ IP �����, ���������� ����� � ������ ����������
				std::string ip_address, physical_address;
				int interface_index;
				std::cout << "IP �����: ";
				std::cout.flush();
				std::cin >> ip_address;
				std::cout << "MAC �����: ";
				std::cout.flush();
				std::cin >> physical_address;
				std::cout << "������ ����������: ";
				std::cout.flush();
				std::cin >> interface_index;

				// �������� ������
				ARPHelper::addArpEntry(ip_address, physical_address, interface_index);
			}
			else if (input == "D") {
				// ������ IP �����, ���������� ����� � ������ ����������
				std::string ip_address, physical_address;
				int interface_index;
				std::cout << "IP �����: ";
				std::cout.flush();
				std::cin >> ip_address;
				std::cout << "MAC �����: ";
				std::cout.flush();
				std::cin >> physical_address;
				std::cout << "������ ����������: ";
				std::cout.flush();
				std::cin >> interface_index;

				// ������� ������
				ARPHelper::deleteArpEntry(ip_address, physical_address, interface_index);
			}
			else if (input == "S") {
				// ������ IP �����
				std::string ip_address;
				std::cout << "IP �����: ";
				std::cout.flush();
				std::cin >> ip_address;

				// ��������� ARP ������
				std::cout << "���������� MAC-�����: " << ARPHelper::sendARP(ip_address) << std::endl;
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
	IServer::detach();


	return 0;
}