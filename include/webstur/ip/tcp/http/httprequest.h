#pragma once

#include <optional>
#include <map>
#include <webstur/utils.h>
#include <webstur/ip/tcp/http/httpmethod.h>

class DLLEXPORT HTTPRequest {
public:
	// ���������� HTTPRequest, ���� ���������� ���������� ��������� 
	// ������� � ����� ������ ������������� �������� � ������, ����� - ������.
	static std::optional<HTTPRequest> parse(const std::string &message, const std::string& pattern, HTTPMethod method);

	// ���������� HTTPRequest, ���� ���������� ���������� ���������
	// �������, ����� - ������.
	static std::optional<HTTPRequest> parse(const std::string& message);

	// ���������� true, ���� ���� � ��������� ��������� � ���������
	static bool matches_path(const std::string& message, const std::string& pattern);

	// ���������� ��������� �������
	const std::map<std::string, std::string>& get_headers();
private:
	HTTPMethod method;
	std::string query;
	std::map<std::string, std::string> headers;

	HTTPRequest();
};