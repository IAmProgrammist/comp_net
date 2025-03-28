#pragma once

#include <WinSock2.h>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <webstur/iserver.h>
#include <webstur/utils.h>

class DLLEXPORT UDPServer : public IServer {
	void wait_for_server_stop();
protected:
	std::atomic<bool>* running = nullptr;
	std::atomic<bool>* should_run = nullptr;
	std::ifstream* file = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;

public:

	// Создаёт сервер
	UDPServer(std::string file_path, int port = UDP_SERVER_DEFAULT_PORT);
	// Освобождает ресурсы сервера
	~UDPServer();
	// Возобновить работу сервера
	void start();
	// Приостановить работу сервера
	void shutdown();
	// Отобразить информацию о сервере
	std::ostream& printServerInfo(std::ostream& out);
};