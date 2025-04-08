#pragma once

#include <thread>
#include <webstur/utils.h>
#include <webstur/iclient.h>

class DLLEXPORT TCPClient : public IClient {
protected:
	std::atomic<bool>* running = nullptr;
	std::atomic<bool>* should_run = nullptr;
	SOCKET socket_descriptor;
	std::thread* current_runner = nullptr;
	std::string server_address;
	int server_port;

	// Метод, вызываемый при установлении соединения с сервером
	virtual void onConnect() = 0;
	// Метод, вызываемый при разрыве соединения с сервером
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	virtual void onDisconnect() = 0;
	// Метод, вызываемый при отправке получении сообщения от сервера
	virtual void onMessage(const std::vector<char>& message) = 0;
	// Отправляет данные message TCP-серверу
	void sendMessage(std::istream& message);
public:
	// Создаёт сервер
	TCPClient(std::string address, int port = TCP_SERVER_DEFAULT_PORT);
	// Освобождает ресурсы клиента
	~TCPClient();
	// Приостановить работу клиента
	void shutdown();
	// Устанавливает соединение с сервером
	void start();
	// Запрашивает данные с сервера
	void request();
	// Запрашивает данные с сервера с дополнительной отправкой данных
	void request(char* payload, int payload_size);
	// Отобразить информацию о клиенте
	std::ostream& printClientInfo(std::ostream& out);
	
private:
	// Синхронное ожидание остановки сервера
	void waitForClientStop();
	// Синхронное ожидание старта сервера
	void waitForClientStart();
	// Метод для работы с сервером
	void connection();
};