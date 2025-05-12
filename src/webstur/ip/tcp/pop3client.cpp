#include "pch.h"
#include <assert.h>
#include <webstur/ip/tcp/pop3client.h>
#include <webstur/utils.h>


std::map<POP3Tasks::POP3Tasks, bool> POP3Client::is_multi_line = {
	{ POP3Tasks::LIST, true },
	{ POP3Tasks::MAIL, true },
	{ POP3Tasks::DEL, false },
	{ POP3Tasks::INIT, false },
	{ POP3Tasks::NONE, false },
	{ POP3Tasks::LOGIN, false },
	{ POP3Tasks::PASSWORD, false },
	{ POP3Tasks::QUIT, false },
};


void POP3Client::onConnect() {
	// Ожидаем, что первое полученное сообщение будет сообщение инициализации
	this->current_task = POP3Tasks::INIT;
}

void POP3Client::onDisconnect() {
	// Ничего не делаем
}

std::string POP3Client::extractNextResponseFromBuffer() {
	// Определить, является ли текущий запрос мультистроковым
	auto is_current_task_multi_line_it = this->is_multi_line.find(this->current_task);
	assert(is_current_task_multi_line_it != this->is_multi_line.end());
	auto is_current_task_multi_line = is_current_task_multi_line_it->second;

	std::size_t break_line_pos;
	std::string response;
	// Задача мультистроковая и ответ положительный
	if ((break_line_pos = this->buffer.find("\r\n.\r\n")) != std::string::npos &&
		is_current_task_multi_line &&
		this->buffer.starts_with("+OK")) {
		response = this->buffer.substr(0, break_line_pos);
		this->buffer = this->buffer.substr(break_line_pos + 5);
		// Задача немультистроковая или мультистроковая, но ответ отрицательный
	}
	else {
		break_line_pos = this->buffer.find("\r\n");
		if (!is_current_task_multi_line && break_line_pos != std::string::npos ||
			is_current_task_multi_line && !this->buffer.starts_with("+OK") &&
			break_line_pos != std::string::npos) {
			response = this->buffer.substr(0, break_line_pos);
			this->buffer = this->buffer.substr(break_line_pos + 2);
		}
	}

	return response;
}

void POP3Client::delegateResponse(const std::string& response) {
	// Сбросить текущую задачу
	auto old_task = this->current_task;
	this->current_task = POP3Tasks::NONE;

	// Если ответ начинается с ошибки, то вызвать метод-обработчик ошибки
	if (response.starts_with("-ERR")) {
		this->onError(old_task, response.substr(5));
		return;
	}
	else {
		switch (old_task) {
		case POP3Tasks::LIST:
		{
			auto splitted_data = split(response, "\r\n");
			std::vector<EMLHeader> headers;
			for (int i = 1; i < splitted_data.size(); i++) {
				auto splitted_header = split(splitted_data[i], " ");
				EMLHeader header = {
					std::atoi(splitted_header[0].c_str()),
					std::atoi(splitted_header[1].c_str())
				};
				headers.push_back(header);
			}
			this->onMailList(headers);
			break;
		}
		case POP3Tasks::MAIL:
		{
			auto break_line_pos = response.find("\r\n");
			EMLContent dat = {
				response.substr(break_line_pos + 2)
			};
			this->onMail(dat);
			break;
		}
		case POP3Tasks::DEL:
			this->onMailDelete();
			break;
		case POP3Tasks::INIT:
			this->onInit();
			break;
		case POP3Tasks::LOGIN:
			this->onLogin();
			break;
		case POP3Tasks::PASSWORD:
			this->onPassword();
			break;
		case POP3Tasks::QUIT:
			this->onQuit();
			break;
		default:
			this->onError(old_task, "Task not implemented");
		}
	}
}

void POP3Client::onMessage(const std::vector<char>& message) {
	// Сохранить ответ в буфер
	this->buffer += std::string(message.begin(), message.end());

	while (true) {
		std::string response = extractNextResponseFromBuffer();

		// Если ответ пустой, выходим из метода
		if (response.empty())
			return;

		// Если текущая задача не задана, также выходим
		if (this->current_task == POP3Tasks::NONE)
			continue;

		this->delegateResponse(response);
	}
}

void POP3Client::request(const char* payload, int payload_size) {
	if (this->current_task != POP3Tasks::NONE)
		return;
	assert(payload_size >= sizeof(POP3Request));
	auto request = ((POP3Request*) payload);
	std::stringstream raw_request;
	this->current_task = request->task;

	switch (request->task) {
	case POP3Tasks::LIST:
	{
		raw_request << "LIST";
		break;
	}
	case POP3Tasks::MAIL:
	{
		raw_request << "RETR " << request->arguments.id;
		break;
	}
	case POP3Tasks::DEL:
		raw_request << "DELE " << request->arguments.id;
		break;
	case POP3Tasks::LOGIN:
		raw_request << "USER " << std::string(request->arguments.login, 
			request->arguments.login + strlen(request->arguments.login));
		break;
	case POP3Tasks::PASSWORD:
		raw_request << "PASS " << std::string(request->arguments.password,
			request->arguments.password + strlen(request->arguments.password));;
		break;
	case POP3Tasks::QUIT:
		raw_request << "QUIT";
		break;
	default:
		this->current_task = POP3Tasks::NONE;
		this->onError(request->task, "Task not implemented");
	}
	raw_request << "\r\n";

	TCPClient::request(raw_request.str().c_str(), raw_request.str().size());
}

POP3Client::POP3Client(std::string address, int port) : 
	TCPClient(address, port) {
	this->current_task = POP3Tasks::NONE;
}