#pragma once

#include <optional>
#include <map>
#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httpmethod.h>

class DLLEXPORT HTTPRequest {
public:
	// ¬озвращает HTTPRequest, если получаетс€ распарсить заголовок 
	// запроса а также запрос соответствует паттерну и методу, иначе - ничего.
	static std::optional<HTTPRequest> parse(const std::string &message, const std::string& pattern, HTTPMethod method);

	// ¬озвращает HTTPRequest, если получаетс€ распарсить заголовок
	// запроса, иначе - ничего.
	static std::optional<HTTPRequest> parse(const std::string& message);

	// ¬озвращает true, если путь в сообщении совпадает с паттерном
	static bool matches_path(const std::string& message, const std::string& pattern);

	// ¬озвращает заголовки запроса
	const std::map<std::string, std::string>& get_headers();
private:
	HTTPMethod method;
	std::string query;
	std::map<std::string, std::string> headers;

	HTTPRequest();
};