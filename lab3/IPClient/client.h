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
	// Создаёт клиента
	Client();
	// Освобождает ресурсы клиента
	virtual ~Client();
	// Приостановить работу клиента
	void shutdown();
	// Запрашивает данные с клиента с дополнительной отправкой данных.
	// payload и payload_size игнорируется в данной реализации
	void request(char* payload, int payload_size);
	// Возвращает true если ответ принят
	bool isReady();
	// Возвращает true если ответ принят
	const std::vector<char>& getAnswer();
	// Отобразить информацию о клиенте
	std::ostream& printClientInfo(std::ostream& out);
};