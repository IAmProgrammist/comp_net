// TCP-������, ����������� ���� �� ������������ � ������������ � ����� ����

#pragma once

#include <webstur/ip/tcp/tcpserver.h>

class DLLEXPORT FileTCPServer : public TCPServer {
protected:
	// �����, ���������� ��� ������������ ���������� � ��������
	virtual void onConnect(SOCKET client);
	// �����, ���������� ��� ������� ���������� � ��������
	virtual void onDisconnect(SOCKET client);
	// ��������� ���� �� ������� � ���������� ���� �� ����� ����
	void onMessage(SOCKET client, const std::vector<char>& message);
};