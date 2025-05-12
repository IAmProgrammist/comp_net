#include <iostream>
#include <fstream>
#include "mypop3client.h"

#define LOG_PREFIX "My POP3 client: "

MyPOP3Client::MyPOP3Client(std::string address, std::string login, std::string password) : POP3Client(address), login(login), password(password) {}

void MyPOP3Client::onMailList(std::vector<EMLHeader> mail_list) {
	std::cout << LOG_PREFIX << "A " << mail_list.size() << " mails are not read\n";
	for (auto& mail : mail_list)
		std::cout << "  ID: " << mail.id << " Size: " << mail.size << " bytes\n";

	std::cout << std::flush;
}

void MyPOP3Client::onMail(const EMLContent& mail_content) {
	std::cout << LOG_PREFIX << "A mail content is received" << std::endl;

	// Получить уникальное имя файла
	auto unique_filename = getUniqueFilepath();
	unique_filename = unique_filename.substr(0, unique_filename.size() - 4) + ".rmf";
	
	// Открыть файл
	std::ofstream out(unique_filename, std::ios::binary);

	if (!out.is_open()) {
		std::cout << "Unable to save a file" << std::endl;
		return;
	}

	// Запись в файл
	out.write(mail_content.data.c_str(), mail_content.data.size());

	// Сохранить и закрыть файл
	out.flush();
	out.close();

	std::cout << LOG_PREFIX << "A mail is saved at '" << unique_filename << "'" << std::endl;
}

void MyPOP3Client::onMailDelete() {
	std::cout << LOG_PREFIX << "A mail is deleted successfully" << std::endl;
}

void MyPOP3Client::onInit() {
	std::cout << LOG_PREFIX << "A POP3 connection is established" << std::endl;
	// Отправить запрос на логин
	POP3Request login_request;
	login_request.task = POP3Tasks::LOGIN;
	login_request.arguments.login = this->login.c_str();
	this->request((const char*)(&login_request), sizeof(login_request));
}

void MyPOP3Client::onLogin() {
	std::cout << LOG_PREFIX << "A login is succedeed succesfully" << std::endl;
	// Отправить запрос на пароль
	POP3Request password_request;
	password_request.task = POP3Tasks::PASSWORD;
	password_request.arguments.password = this->password.c_str();
	this->request((const char*)(&password_request), 
		sizeof(password_request));
}

void MyPOP3Client::onPassword() {
	std::cout << LOG_PREFIX << "A password is succedeed succesfully" << std::endl;
}

void MyPOP3Client::onQuit() {
	std::cout << LOG_PREFIX << "A client quitted, you can exit now" << std::endl;

	// Выключить клиент
	this->shutdown();
}

void MyPOP3Client::onError(POP3Tasks::POP3Tasks failed_task, std::string response) {
	std::cout << LOG_PREFIX << "A POP3 failed with error '" << response <<
		"' for task " << failed_task << std::endl;

	switch (failed_task) {
	case POP3Tasks::LOGIN:
		std::cout << LOG_PREFIX << "Invalid login" << std::endl;
		this->shutdown();
		break;
	case POP3Tasks::PASSWORD:
		std::cout << LOG_PREFIX << "Invalid password" << std::endl;
		this->shutdown();
		break;
	}
}