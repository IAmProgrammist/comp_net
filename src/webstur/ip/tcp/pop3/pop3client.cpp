#include "pch.h"
#include <assert.h>
#include <webstur/ip/tcp/pop3/pop3client.h>
#include <webstur/utils.h>

std::map<POP3Tasks, bool> POP3Client::is_multi_line = {
	{ LIST, true },
	{ MAIL, true },
	{ DEL, false },
	{ INIT, false },
	{ NONE, false },
};


void POP3Client::onConnect() {
	// �������, ��� ������ ���������� ��������� ����� ��������� �������������
	this->current_task = INIT;
}

void POP3Client::onDisconnect() {
	// ������ �� ������
}

void POP3Client::onMessage(const std::vector<char>& message) {
	// ��������� ����� � �����
	this->buffer += std::string(&message[0], sizeof(message));

	// ����������, �������� �� ������� ������ ���������������
	auto is_current_task_multi_line_it = this->is_multi_line.find(this->current_task);
	assert(is_current_task_multi_line_it != this->is_multi_line.end());
	auto is_current_task_multi_line = is_current_task_multi_line_it->second;

	std::size_t break_line_pos;
	std::string response;
	// ������ ��������������� � ����� �������������
	if ((break_line_pos = this->buffer.find("\r\n.\r\n")) != std::string::npos &&
		is_current_task_multi_line &&
		this->buffer.starts_with("+OK")) {
		response = this->buffer.substr(0, break_line_pos);
		this->buffer = this->buffer.substr(break_line_pos + 5);
		// ������ ����������������� ��� ���������������, �� ����� �������������
	}
	else {
		break_line_pos = this->buffer.find("\r\n");
		if (!is_current_task_multi_line && break_line_pos != std::string::npos ||
			is_current_task_multi_line && !this->buffer.starts_with("+OK") &&
			break_line_pos != std::string::npos) {
			response = this->buffer.substr(0, break_line_pos);
			this->buffer = this->buffer.substr(break_line_pos + 5);
		}
	}

	// ���� ����� ������, ������� �� ������
	if (response.empty())
		return;

	// ���� ������� ������ �� ������, ����� �������
	if (this->current_task == NONE)
		return;

	// �������� ������� ������
	auto old_task = this->current_task;
	this->current_task = NONE;

	// ���� ����� ���������� � ������, �� ������� �����-���������� ������
	if (response.starts_with("-ERR")) {
		this->onError(old_task, response.substr(5));
		return;
	}
	else {
		switch (old_task) {
		case LIST:
		{
			auto splitted_data = split(response, "\r\n");
			std::vector<EMLHeader> headers;
			for (int i = 0; i < splitted_data.size(); i++) {
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
		case MAIL:
			break;
		case DEL:
			this->onMailDelete();
			break;
		case INIT:
			this->onInit();
			break;
		default:
			this->onError(old_task, "Task not implemented");
		}
	}
}

void POP3Client::onInit() {

}