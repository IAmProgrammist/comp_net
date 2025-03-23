#pragma once

#include <vector>
#include <thread>
#include "iclient.h"

class Client : public IClient {
	void wait_for_client_stop();
protected:
	std::atomic<bool> running = false;
	std::atomic<bool> should_run = false;
	SOCKET socket_descriptor;
	std::vector<char> temporary_data;
	std::thread* current_runner = nullptr;
public:
	// ������ �������
	Client();
	// ����������� ������� �������
	virtual ~Client();
	// ������������� ������ �������
	void shutdown();
	// ����������� ������ � ������� � �������������� ��������� ������.
	// payload � payload_size ������������ � ������ ����������
	void request(char* payload, int payload_size);
	// ���������� true ���� ����� ������
	bool isReady();
	// ���������� true ���� ����� ������
	const std::vector<char>& getAnswer();
	// ���������� ���������� � �������
	std::ostream& printClientInfo(std::ostream& out);
};