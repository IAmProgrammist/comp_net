#pragma once

#include <vector>
#include "iclient.h"

class Client : public IClient {
protected:
	SOCKET socket_descriptor;
	std::vector<char> temporary_data;
public:
	// ������ �������
	Client();
	// ����������� ������� �������
	virtual ~Client();
	// ����������� ������ � ������� � �������������� ��������� ������.
	// payload � payload_size ������������ � ������ ����������
	void request(char* payload, int payload_size);
	// ��������� ������ ���������� �� �������
	std::string save();
	// ���������� ���������� � �������
	std::ostream& printClientInfo(std::ostream& out);
};