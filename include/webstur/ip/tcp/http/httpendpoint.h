#pragma once

#include <optional>

#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpresponse.h>
#include <webstur/ip/tcp/http/httpmethod.h>

class DLLEXPORT HTTPEndpoint {
private:
	HTTPMethod method;
	std::string path;
public:
	// �������� ���������� HTTP ���������, ��������� ��� ��
	// ��������� ������� �, ���� �� ������, �������� ��������, 
	// ���������� HTTPResponse. ����� - ���������� ������.
	std::optional<HTTPResponse> parse_and_process(const std::string& message);

	bool matches(const std::string& message);
protected:
	// ����� ��� ��������� �������
	virtual HTTPResponse process(const HTTPRequest& request) = 0;
};