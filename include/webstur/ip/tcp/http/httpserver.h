// ������� HTTP-������

#pragma once

#include <vector>

#include <webstur/ip/tcp/http/httpendpoint.h>
#include <webstur/ip/tcp/tcpserver.h>

#define HTTP_DEFAULT_SERVER_PORT 80

class DLLEXPORT HTTPServer : public TCPServer {
private:
	std::vector<HTTPEndpoint> endpoints;
public:
	HTTPServer(int port = HTTP_DEFAULT_SERVER_PORT);

	// �������� � ������ ���������� ����� ��������
	void injectEndpoint(const HTTPEndpoint& endpoint);
	// �����, ���������� ��� ������������ ���������� � ��������
	void onConnect(SOCKET client);
	// �����, ���������� ��� ������� ���������� � ��������
	// ���������� ����� ��� �� �������� ��������������, 
	// ������ ��������� ��� ���������� ����������
	void onDisconnect(SOCKET client);
	// �����, ���������� ��� ������� ������� � ��������
	void onMessage(SOCKET client, const std::vector<char>& message);
};