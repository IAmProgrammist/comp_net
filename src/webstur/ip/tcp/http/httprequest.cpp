#include "pch.h"

#include <webstur/ip/tcp/http/httprequest.h>

HTTPRequest::HTTPRequest() {}

std::optional<HTTPRequest> HTTPRequest::parse(const std::string& message, const std::string& pattern, HTTPMethod method) {

}

std::optional<HTTPRequest> HTTPRequest::parse(const std::string& message) {
	HTTPRequest result;
	
	bool first_line_to_be_extracted = true;
	// Начало строчки
	size_t previous_break_line_pos = 0;
	// Конец строчки
	auto break_line_pos = message.find("\r\n");

	// Получить метод и запрос
	{
		auto slice = std::string_view{ message.c_str(), break_line_pos - previous_break_line_pos };
		auto space_index = slice.find(" ");
		auto method_name = slice.substr(0, space_index);
		if (method_name == "GET")
			result.method = HTTPMethod::GET;
		else if (method_name == "HEAD")
			result.method = HTTPMethod::HEAD;
		else if (method_name == "POST")
			result.method = HTTPMethod::POST;
		else if (method_name == "PATCH")
			result.method = HTTPMethod::PATCH;
		else if (method_name == "DELETE")
			result.method = HTTPMethod::DEL;
		else
			return std::nullopt;

		result.query = slice.substr(space_index + 1);
	}
	
	// Считываем заголовки, до первого \r\n
	while (break_line_pos - previous_break_line_pos > 2 ) {
		if (break_line_pos == std::string::npos) {
			break;
		}
		previous_break_line_pos = break_line_pos + 2;
		break_line_pos = message.find("\r\n", previous_break_line_pos + 2);

		auto slice = std::string_view(message.c_str(), break_line_pos - previous_break_line_pos);
		auto space_index = slice.find(": ");
	}

	// TODO: добавить считывание тела запроса
}

bool HTTPRequest::matches_path(const std::string& message, const std::string& pattern) {

}

// Возвращает заголовки запроса
const std::map<std::string, std::string>& HTTPRequest::get_headers() {
	return this->headers;
}