#include "pch.h"

#include <iostream>
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpserver.h>

HTTPServer::HTTPServer(int port) : TCPServer(port) {};

void HTTPServer::injectEndpoint(HTTPEndpoint& endpoint) {
	// Внести эндпоинт в список эндпоинтов
	this->endpoints.push_back(&endpoint);
}

void HTTPServer::onConnect(SOCKET client) {
	std::clog << "A new HTTP server connection established" << std::endl;
	
	// Внести временную информацию для сессии: буферы, длину контента, позицию конца заголовка
	this->buffers.insert(std::pair{client, ""});
	this->content_lengths.insert({ client, std::string::npos });
	this->header_end_poses.insert({ client, std::string::npos });
}

void HTTPServer::onDisconnect(SOCKET client) {
	std::clog << "An HTTP connection is ended" << std::endl;

	// Удалить временную информацию для сессии
	this->buffers.erase(this->buffers.find(client));
	this->content_lengths.erase(this->content_lengths.find(client));
	this->header_end_poses.erase(this->header_end_poses.find(client));
}

void HTTPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	// Получить временную информацию для сессии
	std::string& buffer = this->buffers.find(client)->second;
	std::size_t& content_length = this->content_lengths.find(client)->second;
	std::size_t& header_end_pos = this->header_end_poses.find(client)->second;

	// Дополнить буфер
	buffer += std::string(message.begin(), message.end());

	// Если позиция конца заголовка ещё неизвестна
	if (header_end_pos == std::string::npos) {
		// Попытаться найти позицию конца заголовка
		header_end_pos = buffer.find("\r\n\r\n");

		// Если позиции не найдено, выйти из метода
		if (header_end_pos == std::string::npos)
			return;

		// Распарсить заголовок
		auto opt_parsed_header = HTTPRequest::parse(buffer.substr(0, header_end_pos + 4));
		
		// Если заголовок невалиден
		if (!opt_parsed_header.isValid()) {
			// Отправить запрос с кодом 422 и выйти
			HTTPResponse response_422 = HTTPResponse(422);
			sendMessageAndDisconnect(client, response_422);
			return;
		}

		// Постараться получить длину запроса
		auto headers = opt_parsed_header.getHeaders();
		auto content_length_it = headers.find("Content-Length");

		// Если длина запроса найдена
		if (content_length_it != headers.end()) {
			// Обновить длину заголовка для сессии, перезапустить метод onMessage
			// для обработки с установленным content_length на случай, 
			// если клиент пришлёт тело с заголовками в одном сообщении
			content_length = std::atoi(content_length_it->second.c_str());
			std::vector<char> empty_string;
			this->onMessage(client, empty_string);
		}
		else {
			// Вызвать метод-колбек для запроса: если не установлена длина, 
			// то тело запроса игнорируется
			onRequest(client, opt_parsed_header);
		}
	}
	else {
		// Если длина тела достигла Content-Length
		if (buffer.size() - header_end_pos + 4 >= content_length) {
			// Распарсить запрос
			auto request = HTTPRequest::parse(buffer);

			// Вызвать метод-колбек
			onRequest(client, request);
		}
	}
}

void HTTPServer::onRequest(SOCKET client, HTTPRequest& message) {
	HTTPEndpoint* endpoint = nullptr;
	
	// Найти эндпоинт, для которого удовлетворяет путь запроса
	for (auto& p_endpoint : this->endpoints) {
		if (p_endpoint->matches(message)) {
			endpoint = p_endpoint;
			break;
		}
	}

	// Если запрос не найден, возвращаем 404
	if (endpoint == nullptr) {
		HTTPResponse response_404 = HTTPResponse(404);
		sendMessageAndDisconnect(client, response_404);
		return;
	}

	// Если метод HEAD, возвращаем 200
	if (message.getMethod() == HEAD) {
		HTTPResponse response_200 = HTTPResponse(200);
		sendMessageAndDisconnect(client, response_200);
		return;
	}

	// Получить ответ от эндпоинта и отправить его
	auto response = endpoint->process(message);
	sendMessageAndDisconnect(client, *response);
	delete response;
}

void HTTPServer::sendMessageAndDisconnect(SOCKET client, const HTTPResponse& response) {
	// Сериаилазовать ответ
	auto response_stream = response.serialize();
	// Отправить ответ
	this->sendMessage(client, response_stream);
	// Разорвать соединение
	this->disconnect(client);
}