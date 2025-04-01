// ѕростой TCP-сервер, вывод€щий сообщени€ от пользовател€ в консоль

#pragma once

#include <webstur/ip/tcp/tcpserver.h>

class DLLEXPORT SimpleTCPServer : public TCPServer {
protected:
	// ћетод, вызываемый при установлении соединени€ с клиентом
	virtual void onConnect(SOCKET client);
	// ћетод, вызываемый при разрыве соединени€ с клиентом
	virtual void onDisconnect(SOCKET client);
	// ¬ыводит любое сообщение, приход€щее от клиента
	void onMessage(SOCKET client, const std::vector<char>& message);
};