#pragma once

#include <string>
#include <vector>
#include <webstur/utils.h>

class DLLEXPORT IClient {
public:
	// Создаёт клиента
	IClient();
	// Освобождает ресурсы клиента
	virtual ~IClient();
	// Приостановить работу клиента
	virtual void shutdown() = 0;
	// Запрашивает данные с сервера
	virtual void request();
	// Запрашивает данные с сервера с дополнительной отправкой данных
	virtual void request(char* payload, int payload_size) = 0;
	// Отобразить информацию о клиенте
	virtual std::ostream& printClientInfo(std::ostream& out) = 0;

	// Загружает библиотеку WinSock
	static void init();
	// Выгружает библиотеку WinSock
	static void detach();
};