#include "cardendpoint.h"

// Сконструировать эндпоинт по пути /my-card
CardEndpoint::CardEndpoint() : HTTPEndpoint("/my-card", GET) {};

HTTPResponse* CardEndpoint::process(const HTTPRequest& request) {
	// Выдать файл-визитку
	HTTPResponse* answer = new HTTPFileResponse(200, ".\\assets\\index.html");

	return answer;
}