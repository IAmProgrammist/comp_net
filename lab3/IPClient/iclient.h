#pragma once

#include <string>
#include <vector>

class IClient {
public:
	// Создаёт клиента
	IClient();
	// Освобождает ресурсы клиента
	virtual ~IClient();
	// Приостановить работу клиента
	virtual void shutdown() = 0;
	// Запрашивает данные с сервера
	void request();
	// Запрашивает данные с сервера с дополнительной отправкой данных
	virtual void request(char* payload, int payload_size) = 0;
	// Возвращает true если ответ принят
	virtual bool isReady() = 0;
	// Возвращает массив с информацией принятой от сервера
	virtual const std::vector<char>& getAnswer() = 0;
	// Отобразить информацию о клиенте
	virtual std::ostream& printClientInfo(std::ostream& out) = 0;
	// Отобразить информацию о файле
	virtual std::ostream& printAnswerInfo(std::ostream& out) = 0;

	// Загружает библиотеку WinSock
	static void init();
	// Выгружает библиотеку WinSock
	static void detach();
};