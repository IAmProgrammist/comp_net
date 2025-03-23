#pragma once

#include <string>

// Абстрактный класс для работы с WinSock библиотекой
class IServer {
public:
	// Создаёт сервер
	IServer();
	// Освобождает ресурсы сервера
	virtual ~IServer() = 0;
	// Возобновить работу сервера
	virtual void start() = 0;
	// Приостановить работу сервера
	virtual void shutdown() = 0;
	// Отобразить информацию о сервере
	virtual std::ostream& printServerInfo(std::ostream& out) = 0;

	// Загружает библиотеку WinSock
	static void init();
	// Выгружает библиотеку WinSock
	static void detach();
};
