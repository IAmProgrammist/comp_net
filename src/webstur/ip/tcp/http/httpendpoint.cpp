#include "pch.h"
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpendpoint.h>

std::optional<HTTPResponse> HTTPEndpoint::parse_and_process(const std::string& message) {
	// ���������� ������
	auto request_opt = HTTPRequest::parse(message, this->path);
	
	// ���� ���������� �� �������, ����� �� ������
	if (!request_opt.has_value())
		return std::nullopt;

	auto request = request_opt.value();
}