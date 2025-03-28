// Класс для инициализации широковещательного UDP-сервера

#pragma once

#include <vector>
#include <thread>
#include <webstur/iclient.h>
#include <webstur/utils.h>

class DLLEXPORT Client : public IClient {
	void wait_for_client_stop();
protected:
	std::atomic<bool> running = false;
	std::atomic<bool> should_run = false;
	SOCKET socket_descriptor;
	std::vector<char> temporary_data;
	std::thread* current_runner = nullptr;
public:
	// Создаёт клиент
	Client();
	// Освобождает ресурсы клиента
	virtual ~Client();
	// Приостановить принятие информации от сервера
	void shutdown();
	// Запускает поток, принимающий пакеты от сервера.
	// payload и payload_size игнорируется в данной реализации
	void request(char* payload, int payload_size);
	// Возвращает true если вся информация от сервера была принята
	bool isReady();
	// Возвращает массив с информацией принятой от сервера
	const std::vector<char>& getAnswer();
	// Отобразить информацию о клиенте
	std::ostream& printClientInfo(std::ostream& out);
	// Отобразить информацию о принятой информации от сервера
	std::ostream& printAnswerInfo(std::ostream& out);
};