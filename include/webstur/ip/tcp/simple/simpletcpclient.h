#pragma once

#include <webstur/ip/tcp/tcpclient.h>

class DLLEXPORT SimpleTCPClient : public TCPClient {
public:
	SimpleTCPClient(std::string address, int port = TCP_SERVER_DEFAULT_PORT);
protected:
	// Метод, вызываемый при установлении соединения с сервером
	virtual void onConnect();
	// Метод, вызываемый при разрыве соединения с сервером
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	virtual void onDisconnect();
	// Метод, вызываемый при отправке получении сообщения от сервера
	// Выводит сообщение в консоль
	virtual void onMessage(const std::vector<char>& message);
};