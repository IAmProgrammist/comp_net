#pragma once

#include <map>
#include <sstream>
#include <string>
#include <webstur/utils.h>

#define HTTP_VERSION "HTTP/1.1"
#define SERVER_NAME "Webstur"

class DLLEXPORT HTTPResponse {
public:
	std::map<std::string, std::string> headers;
	int code;

	HTTPResponse(int code);
	virtual ~HTTPResponse();
	// Возвращает стрим для ответа
	virtual std::stringstream serialize() const;
};

class DLLEXPORT HTTPFileResponse : public HTTPResponse {
public:
	std::string file_path;
	std::string contents;

	HTTPFileResponse(int code, std::string file_path);
	virtual ~HTTPFileResponse();

	// Возвращает стрим для ответа
	std::stringstream serialize()  const;
};