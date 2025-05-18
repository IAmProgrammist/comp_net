#include "pch.h"

#include <webstur/ip/tcp/http/httprequest.h>

HTTPRequest::HTTPRequest() {}

HTTPRequest HTTPRequest::parse(const std::string& message) {
	HTTPRequest result;
	
	bool first_line_to_be_extracted = true;
	// Начало строчки
	size_t previous_break_line_pos = 0;
	// Конец строчки
	auto break_line_pos = message.find("\r\n");

	// Получить метод и запрос
	{
		auto splitted = split(message.substr(0, break_line_pos), " ");
		if (splitted.size() < 2) {
			result.is_valid = false;
			return result;
		}
		
		auto method_name = splitted[0];
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
		else {
			result.is_valid = false;
			return result;
		}

		result.query = splitted[1];
	}
	
	auto message_size = message.size();

	// Считываем заголовки, до первого \r\n\r\n
	while (true) {
		if (break_line_pos == std::string::npos) {
			result.is_valid = false;
			return result;
		}
		previous_break_line_pos = break_line_pos + 2;
		break_line_pos = message.find("\r\n", previous_break_line_pos);

		if (break_line_pos - previous_break_line_pos <= 2) {
			break;
		}

		auto slice = std::string_view(message.c_str() + previous_break_line_pos, 
			custom_min(break_line_pos, message_size) - previous_break_line_pos);
		auto space_index = slice.find(": ");
		auto header_name = slice.substr(0, space_index);
		auto header_val = slice.substr(space_index + 2);
		result.headers.insert(std::pair{ header_name, header_val });
	}

	result.body = message.substr(break_line_pos + 2);
	result.is_valid = true;

	return result;

	// TODO: добавить считывание тела запроса
}

bool HTTPRequest::matchesPath(const std::string& pattern) const {
	// TODO: сделать продвинутый распознаватель по паттерну, поддерживающий
	// PATH-переменные
	
	return this->query.starts_with(pattern);
}

const std::map<std::string, std::string>& HTTPRequest::getHeaders() const {
	return this->headers;
}

bool HTTPRequest::isValid() {
	return this->is_valid;
}

const std::string& HTTPRequest::getBody() const {
	return this->body;
}

HTTPMethod HTTPRequest::getMethod() const {
	return this->method;
}