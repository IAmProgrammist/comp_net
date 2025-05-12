// POP3-клиент для консольного приложения

#pragma once

#include <webstur/ip/tcp/pop3client.h>

class MyPOP3Client : public POP3Client {
	std::string login;
	std::string password;
public:
	MyPOP3Client(std::string address, std::string login, std::string password);

	void onMailList(std::vector<EMLHeader> mail_list);

	void onMail(const EMLContent& mail_content);

	void onMailDelete();

	void onInit();

	void onLogin();

	void onPassword();

	void onQuit();

	void onError(POP3Tasks::POP3Tasks failed_task, std::string response);
};