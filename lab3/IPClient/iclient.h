#pragma once

#include <string>

class IClient {
public:
	// ������ �������
	IClient();
	// ����������� ������� �������
	virtual ~IClient();
	// ����������� ������ � �������
	void request();
	// ����������� ������ � ������� � �������������� ��������� ������
	virtual void request(char* payload, int payload_size) = 0;
	// ��������� ������ ���������� �� �������
	virtual std::string save() = 0;
	// ���������� ���������� � �������
	virtual std::ostream& printClientInfo(std::ostream& out) = 0;

	// ��������� ���������� WinSock
	static void init();
	// ��������� ���������� WinSock
	static void detach();
};