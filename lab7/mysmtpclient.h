#pragma once

#include <istream>
#include <webstur/ip/tcp/smtpclient.h>

class MySMTPClient : public SMTPClient {
	std::string login;
	std::string receiver;
	std::istream* data;
public:
	MySMTPClient(std::string address);

	void setLogin(const std::string& login);
	
	void setReceiver(const std::string& receiver);
	
	void setDataStream(std::istream& istream);

	void onHello();

	void onMail();

	void onRecipient();

	void onData();

	void onDataRaw();

	void onInit();

	virtual	void onQuit();

	void onError(SMTPTasks::SMTPTasks failed_task, std::string response);
};