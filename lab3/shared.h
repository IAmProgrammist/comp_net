#pragma once

#include <string>
#include <WinSock2.h>

#define SERVER_DEFAULT_PORT 0
#define CLIENT_DEFAULT_PORT 12484
#define IMAGE_FRAGMENT_SIZE 508

// ��������� ����� ������ ������ � WSA �����
std::string getErrorTextWithWSAErrorCode(std::string errorText);
// ������� ���������� � ��������� sockaddr
std::ostream& printSockaddrInfo(std::ostream& out, sockaddr_in& sock);
// ���������� IP ����� ��� �������� ��
sockaddr_in getDeviceAddrInfo();
// ��������� WSA
void loadWSA();
// ��������� WSA
void unloadWSA();
// ���������� ��������������� ��� ��� �������� �����
std::string getUniqueFilepath();