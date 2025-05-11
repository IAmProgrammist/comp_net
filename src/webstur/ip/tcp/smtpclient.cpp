#include "pch.h"
#include <assert.h>
#include <webstur/ip/tcp/smtpclient.h>
#include <webstur/utils.h>


void SMTPClient::onConnect() {
	// Ожидаем, что первое полученное сообщение будет сообщение инициализации
	this->current_task = INIT;
}

void SMTPClient::onDisconnect() {
	// Ничего не делаем
}

std::string SMTPClient::extractNextResponseFromBuffer() {
	std::size_t break_line_pos;
	std::string response;
	break_line_pos = this->buffer.find("\r\n");
	if (break_line_pos != std::string::npos) {
		response = this->buffer.substr(0, break_line_pos);
		this->buffer = this->buffer.substr(break_line_pos + 2);
	}

	return response;
}

void SMTPClient::delegateResponse(const std::string& response) {
	// Сбросить текущую задачу
	auto old_task = this->current_task;
	this->current_task = NONE;

	// Если ответ начинается с ошибки, то вызвать метод-обработчик ошибки
	if (response.starts_with("4") || response.starts_with("5")) {
		this->onError(old_task, response);
		return;
	}

	else {
		switch (old_task) {
		case HELO:
			this->onHelo();
			break;
		case MAIL:
			this->onMail();
			break;
		case RCPT:
			this->onRcpt();
			break;
		case DATA:
			this->onData();
			break;
		case DATA_RAW:
			this->onDataRaw();
			break;
		case QUIT:
			this->onQuit();
			break;
		default:
			this->onError(old_task, "Task not implemented");
		}
	}
}

void SMTPClient::onMessage(const std::vector<char>& message) {
	// Сохранить ответ в буфер
	this->buffer += std::string(message.begin(), message.end());

	while (true) {
		std::string response = extractNextResponseFromBuffer();

		// Если ответ пустой, выходим из метода
		if (response.empty())
			return;

		// Если текущая задача не задана, также выходим
		if (this->current_task == NONE)
			continue;

		this->delegateResponse(response);
	}
}

void SMTPClient::request(const char* payload, int payload_size) {
	if (this->current_task != NONE)
		return;
	assert(payload_size >= sizeof(POP3Request));
	auto request = ((SMTPRequest*)payload);
	std::stringstream raw_request;
	this->current_task = request->task;
	std::string arg_data = request->payload ?
		std::string(request->payload, request->payload + request->payload_size) : "";

	switch (request->task) {
	case HELO:
		raw_request << "HELO " << arg_data;
		break;
	case MAIL:
		raw_request << "MAIL FROM:<" << arg_data << ">";
		break;
	case RCPT:
		raw_request << "RCPT TO:<" << arg_data << ">";
		break;
	case DATA:
		raw_request << "DATA";
		break;
	case DATA_RAW:
		raw_request << arg_data << "\r\n.";
		break;
	case QUIT:
		raw_request << "QUIT";
		break;
	default:
		this->current_task = NONE;
		this->onError(request->task, "Task not implemented");
	}
	raw_request << "\r\n";

	TCPClient::request(raw_request.str().c_str(), raw_request.str().size());
}

SMTPClient::SMTPClient(std::string address, int port) :
	TCPClient(address, port) {
	this->current_task = NONE;
}