#pragma once

#include <string>
#include <vector>

class IClient {
public:
	// ������ �������
	IClient();
	// ����������� ������� �������
	virtual ~IClient();
	// ������������� ������ �������
	virtual void shutdown() = 0;
	// ����������� ������ � �������
	void request();
	// ����������� ������ � ������� � �������������� ��������� ������
	virtual void request(char* payload, int payload_size) = 0;
	// ���������� true ���� ����� ������
	virtual bool isReady() = 0;
	// ���������� true ���� ����� ������
	virtual const std::vector<char>& getAnswer() = 0;
	// ���������� ���������� � �������
	virtual std::ostream& printClientInfo(std::ostream& out) = 0;

	// ��������� ���������� WinSock
	static void init();
	// ��������� ���������� WinSock
	static void detach();
};