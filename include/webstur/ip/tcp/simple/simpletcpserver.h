// Простой TCP-сервер, выводящий сообщения от пользователя в консоль

#pragma once

#include <webstur/ip/tcp/tcpserver.h>

class DLLEXPORT SimpleTCPServer : public TCPServer {
protected:
	// Метод, вызываемый при установлении соединения с клиентом
	virtual void onConnect(SOCKET client);
	// Метод, вызываемый при разрыве соединения с клиентом
	virtual void onDisconnect(SOCKET client);
	// Выводит любое сообщение, приходящее от клиента
	void onMessage(SOCKET client, const std::vector<char>& message);
};