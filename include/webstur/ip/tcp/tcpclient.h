#pragma once

#include <thread>
#include <webstur/utils.h>
#include <webstur/iclient.h>

class DLLEXPORT TCPClient : public IClient {
	std::atomic<bool>* running = nullptr;
	std::atomic<bool>* should_run = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;

	// Метод, вызываемый при установлении соединения с сервером
	virtual void onConnect(SOCKET server) = 0;
	// Метод, вызываемый при разрыве соединения с сервером
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	virtual void onDisconnect(SOCKET server) = 0;
	// Метод, вызываемый при отправке получении сообщения от сервера
	virtual void onMessage(SOCKET server, const std::vector<char>& message) = 0;
	// Отправляет данные message TCP-серверу server
	void sendMessage(SOCKET server, std::istream& message);
	// Закрывает соединение
	void disconnect(SOCKET client);
public:
	// Создаёт сервер
	TCPClient(std::string address, int port = TCP_SERVER_DEFAULT_PORT);
	// Освобождает ресурсы клиента
	~TCPClient();
	// Возобновить работу клиента
	void start();
	// Приостановить работу клиеента
	void shutdown();
	// Отобразить информацию о клиенте
	std::ostream& printClientInfo(std::ostream& out);

private:
	// Синхронное ожидание остановки сервера
	void waitForClientStop();
};