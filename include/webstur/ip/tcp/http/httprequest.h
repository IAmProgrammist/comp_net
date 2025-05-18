#pragma once
#include <map>
#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httpmethod.h>

#define custom_min(a,b)            (((a) < (b)) ? (a) : (b))
#define custom_max(a,b)            (((a) > (b)) ? (a) : (b))

class DLLEXPORT HTTPRequest {
public:
	bool isValid();

	// Возвращает HTTPRequest, если получается распарсить заголовок
	// запроса, иначе - ничего.
	static HTTPRequest parse(const std::string& message);

	// Возвращает true, если путь в сообщении совпадает с паттерном
	bool matchesPath(const std::string& pattern) const;

	// Возвращает заголовки запроса
	const std::map<std::string, std::string>& getHeaders() const;

	// Возвращает тело запроса
	const std::string& getBody() const;

	// Возвращает метод
	HTTPMethod getMethod() const;
private:
	bool is_valid = true;
	HTTPMethod method;
	std::string query;
	std::map<std::string, std::string> headers;
	std::string body;

	HTTPRequest();
};