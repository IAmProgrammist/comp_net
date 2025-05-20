#pragma once

#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpresponse.h>
#include <webstur/ip/tcp/http/httpmethod.h>

// Класс, описывающий эндпоинт. Задаёт паттерн пути а также метод
class DLLEXPORT HTTPEndpoint {
private:
	HTTPMethod method;
	std::string path;
public:
	// Возвращает true, если путь message соответствует паттерну эндпоинта
	bool matches(const HTTPRequest& message);

	// Конструктор-копия
	HTTPEndpoint(HTTPEndpoint& endpoint);

	// Конструктор, задающий паттерн и метод
	HTTPEndpoint(std::string path, HTTPMethod method);

	// Метод для обработки запроса. 
	// 
	// Внимание! Возвращаемый ответ должен быть
	// в heap (используйте new), память освободится автоматически
	// после отправки ответа.
	virtual HTTPResponse* process(const HTTPRequest& request) = 0;
protected:
};