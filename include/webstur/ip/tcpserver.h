// ����� ��� ������� T�P-�������, ����� ������� �����

#pragma once

#include <atomic>
#include <thread>
#include <semaphore>
#include <webstur/utils.h>
#include <webstur/iserver.h>

class DLLEXPORT TCPServer : public IServer {
protected:
	std::atomic<bool>* running = nullptr;
	std::atomic<bool>* should_run = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;
	std::atomic<int>* current_threads_amount = nullptr;

	// �������-�������, ���������� ��� ��������� ��������� �� �������
	virtual void onMessage(SOCKET client, const std::vector<char>& message) = 0;
public:
	// ������ ������
	TCPServer(int port = TCP_SERVER_DEFAULT_PORT);
	// ����������� ������� �������
	~TCPServer();
	// ����������� ������ �������
	void start();
	// ������������� ������ �������
	void shutdown();
	// ���������� ���������� � �������
	std::ostream& printServerInfo(std::ostream& out);

private:
	// ����� ��� �������� ���������� �� �������
	void serve();
	// ����� ��� ������ � ���������� ��������
	void serveClient(SOCKET client);
	// ���������� �������� ��������� �������
	void waitForServerStop();
};