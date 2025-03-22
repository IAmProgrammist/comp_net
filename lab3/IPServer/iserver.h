#pragma once

#include <string>

// ����������� ����� ��� ������ � WinSock �����������
class IServer {
public:
	// ������ ������
	IServer();
	// ����������� ������� �������
	virtual ~IServer() = 0;
	// ����������� ������ �������
	virtual void start() = 0;
	// ������������� ������ �������
	virtual void shutdown() = 0;
	// ���������� ���������� � �������
	virtual std::ostream& printServerInfo(std::ostream& out) = 0;

	// ��������� ���������� WinSock
	static void init();
	// ��������� ���������� WinSock
	static void detach();
};
