#pragma once

#include <ostream>
#include <webstur/utils.h>

class DLLEXPORT ARPHelper {
public:
	// Выводит таблицу ARP в указанный ostream
	static std::ostream& printARPTable(std::ostream& out);
	// Добавляет запись в ARP таблицу, где
	//   ip               адрес, строка в формате 255.255.255.255
	//   address          физический адрес, строка в формате ff:ff:ff:ff:ff:ff
	//   interface_index  индекс интерфейса 
	static void addArpEntry(std::string ip, std::string address, int interface_index);
	// Удаляет запись из ARP таблицы, где
	//   ip               адрес, строка в формате 255.255.255.255
	//   address          физический адрес, строка в формате ff:ff:ff:ff:ff:ff
	//   interface_index  индекс интерфейса 
	static void deleteArpEntry(std::string ip, std::string address, int interface_index);
	// Отправляет ARP запрос, где
	//   find             адрес, который нужно найти
	static std::string sendARP(std::string find);
    // Форматирует физический адрес
	static std::string prettyPrintPhysicalAddress(unsigned char* addr, int addr_size);
	// Возвращает IP адрес интерфейса по его индексу
	static std::string getInterfaceIpAddressByIndex(int dw_index);
};