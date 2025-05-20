#include "pch.h"
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpendpoint.h>

bool HTTPEndpoint::matches(const HTTPRequest& message) {
	// Если метод - HEAD или метод эндпоинта с запросом совпадают а также
	// расположение запроса совпадает с заданным
	return (message.getMethod() == HEAD || message.getMethod() == this->method)
		&& message.matchesPath(this->path);
}

HTTPEndpoint::HTTPEndpoint(HTTPEndpoint& endpoint) {
	this->path = endpoint.path;
	this->method = endpoint.method;
}

HTTPEndpoint::HTTPEndpoint(std::string path, HTTPMethod method) : path(path), method(method) {};