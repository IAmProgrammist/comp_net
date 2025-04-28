#pragma once

#include <ostream>
#include <webstur/utils.h>

class DLLEXPORT DHCPHelper {
public:
	// Возвращает информацию об IP адресе по доменному имени
	//   name             доменное имя
	static addrinfo* getAddrInfo(std::string name);
	// Возвращает информацию о доменном имени по IP адресу
	//   addrinfo         информация об адресе
	static std::string getNameInfo(sockaddr_in ip);
	// Выводит информацию о DHCP-сервере
	static void printDHCPServerInfo(std::ostream& out);
	// Обновить IP адрес при помощи DHCP
	static void renewIP();
	// Загружает библиотеку DHCP
	static void init();
	// Выгружает библиотеку DHCP
	static void detach();
};