// Простой HTTP-сервер

#pragma once

#include <vector>

#include <webstur/ip/tcp/http/httpendpoint.h>
#include <webstur/ip/tcp/tcpserver.h>

#define HTTP_DEFAULT_SERVER_PORT 80

class DLLEXPORT HTTPServer : public TCPServer {
private:
	std::vector<HTTPEndpoint*> endpoints;
	// Буфер накопления ответов с сервера
	std::map<SOCKET, std::string> buffers;
	std::map<SOCKET, std::size_t> content_lengths;
	std::map<SOCKET, std::size_t> header_end_poses;
public:
	HTTPServer(int port = HTTP_DEFAULT_SERVER_PORT);

	// Включает в список эндпоинтов новый эндпоинт
	void injectEndpoint(HTTPEndpoint& endpoint);
	// Метод, вызываемый при установлении соединения с клиентом
	void onConnect(SOCKET client);
	// Метод, вызываемый при разрыве соединения с клиентом
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	void onDisconnect(SOCKET client);
	// Метод, вызываемый при разрыве общения с клиентом
	void onMessage(SOCKET client, const std::vector<char>& message);
	// Метод, вызываемый при получении валидного запроса. 
	// выполняет роутинг запроса в нужный эндпоинт
	void onRequest(SOCKET client, HTTPRequest& request);
	// Метод, отправляющий сообщение и разрывающий соединение
	void sendMessageAndDisconnect(SOCKET client, const HTTPResponse& response);
};