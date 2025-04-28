#pragma once

#include <ostream>
#include <webstur/utils.h>

class DLLEXPORT DHCPHelper {
public:
	// ���������� ���������� �� IP ������ �� ��������� �����
	//   name             �������� ���
	static addrinfo* getAddrInfo(std::string name);
	// ���������� ���������� � �������� ����� �� IP ������
	//   addrinfo         ���������� �� ������
	static std::string getNameInfo(sockaddr_in ip);
	// ������� ���������� � DHCP-�������
	static void printDHCPServerInfo(std::ostream& out);
	// �������� IP ����� ��� ������ DHCP
	static void renewIP();
	// ��������� ���������� DHCP
	static void init();
	// ��������� ���������� DHCP
	static void detach();
};