#pragma once

#include <string>

class IClient {
public:
	// Создаёт клиента
	IClient();
	// Освобождает ресурсы клиента
	virtual ~IClient();
	// Запрашивает данные с клиента
	void request();
	// Запрашивает данные с клиента с дополнительной отправкой данных
	virtual void request(char* payload, int payload_size) = 0;
	// Сохраняет данные полученные от сервера
	virtual std::string save() = 0;
	// Отобразить информацию о клиенте
	virtual std::ostream& printClientInfo(std::ostream& out) = 0;

	// Загружает библиотеку WinSock
	static void init();
	// Выгружает библиотеку WinSock
	static void detach();
};