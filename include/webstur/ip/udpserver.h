#pragma once

#include <WinSock2.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <webstur/iserver.h>
#include <webstur/utils.h>

class DLLEXPORT Server : public IServer {
	void wait_for_server_stop();
protected:
	std::atomic<bool> running = false;
	std::atomic<bool> should_run = false;
	std::ifstream* file = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;

public:

	// Создаёт сервер
	Server(std::string file_path, int port = UDP_SERVER_DEFAULT_PORT);
	// Освобождает ресурсы сервера
	~Server();
	// Возобновить работу сервера
	void start();
	// Приостановить работу сервера
	void shutdown();
	// Отобразить информацию о сервере
	std::ostream& printServerInfo(std::ostream& out);
};