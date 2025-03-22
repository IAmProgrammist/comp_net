#pragma once

#include <WinSock2.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include "iserver.h"
#include "../shared.h"

class Server : public IServer {
	void wait_for_server_stop();
protected:
	std::atomic<bool> running = false;
	std::atomic<bool> should_run = false;
	std::ifstream* file = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;

public:

	// ������ ������
	Server(std::string file_path, int port = SERVER_DEFAULT_PORT);
	// ����������� ������� �������
	~Server();
	// ����������� ������ �������
	void start();
	// ������������� ������ �������
	void shutdown();
	// ���������� ���������� � �������
	std::ostream& printServerInfo(std::ostream& out);
};