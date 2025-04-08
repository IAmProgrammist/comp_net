// TCP-сервер, принимающий путь от пользователя и отправляющий в ответ файл

#pragma once

#include <webstur/ip/tcp/tcpserver.h>

class DLLEXPORT FileTCPServer : public TCPServer {
protected:
	// Метод, вызываемый при установлении соединения с клиентом
	virtual void onConnect(SOCKET client);
	// Метод, вызываемый при разрыве соединения с клиентом
	virtual void onDisconnect(SOCKET client);
	// Принимает путь от клиента и отправляет файл по этому пути
	void onMessage(SOCKET client, const std::vector<char>& message);
};