// ������� TCP-������, ��������� ��������� �� ������������ � �������

#pragma once

#include<webstur/ip/tcpserver.h>

class DLLEXPORT SimpleTCPServer : public TCPServer {
protected:
	// ������� ����� ���������, ���������� �� �������
	void onMessage(SOCKET client, const std::vector<char>& message);
};