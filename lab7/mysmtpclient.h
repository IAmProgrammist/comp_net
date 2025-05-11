#pragma once

#include <istream>
#include <webstur/ip/tcp/smtpclient.h>

class MySMTPClient : public SMTPClient {
	std::string login;
	std::string receiver;
	std::istream& data;
};