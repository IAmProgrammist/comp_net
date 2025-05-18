#pragma once

#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpresponse.h>
#include <webstur/ip/tcp/http/httpmethod.h>

class DLLEXPORT HTTPEndpoint {
private:
	HTTPMethod method;
	std::string path;
public:
	bool matches(const HTTPRequest& message);

	HTTPEndpoint(HTTPEndpoint& endpoint);

	HTTPEndpoint(std::string path, HTTPMethod method);

	// Метод для обработки запроса. Возвращаемый ответ должен быть
	// в heap (используйте new)
	virtual HTTPResponse* process(const HTTPRequest& request) = 0;
protected:
};