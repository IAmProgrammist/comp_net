// Класс для запуска TСP-сервера, отдаёт обычные файлы

#pragma once

#include <atomic>
#include <thread>
#include <semaphore>
#include <sstream>
#include <webstur/utils.h>
#include <webstur/iserver.h>

class DLLEXPORT TCPServer : public IServer {
protected:
	std::atomic<bool>* running = nullptr;
	std::atomic<bool>* should_run = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;
	std::atomic<int>* current_threads_amount = nullptr;

	// Метод, вызываемый при установлении соединения с клиентом
	virtual void onConnect(SOCKET client) = 0;
	// Метод, вызываемый при разрыве соединения с клиентом
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	virtual void onDisconnect(SOCKET client) = 0;
	// Метод, вызываемый при разрыве общения с клиентом
	virtual void onMessage(SOCKET client, const std::vector<char>& message) = 0;
	// Отправляет данные message TCP-клиенту client
	void sendMessage(SOCKET client, std::istream& message);
	// Закрывает соединение
	void disconnect(SOCKET client);
public:
	// Создаёт сервер
	TCPServer(int port = TCP_SERVER_DEFAULT_PORT);
	// Освобождает ресурсы сервера
	~TCPServer();
	// Возобновить работу сервера
	void start();
	// Приостановить работу сервера
	void shutdown();
	// Отобразить информацию о сервере
	std::ostream& printServerInfo(std::ostream& out);

private:
	// Метод для принятия соединений от клиента
	void serve();
	// Метод для работы с конкретным клиентом
	void serveClient(SOCKET client);
	// Синхронное ожидание остановки сервера
	void waitForServerStop();
};