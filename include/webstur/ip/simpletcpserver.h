// Простой TCP-сервер, выводящий сообщения от пользователя в консоль

#pragma once

#include<webstur/ip/tcpserver.h>

class DLLEXPORT SimpleTCPServer : public TCPServer {
protected:
	// Выводит любое сообщение, приходящее от клиента
	void onMessage(SOCKET client, const std::vector<char>& message);
};