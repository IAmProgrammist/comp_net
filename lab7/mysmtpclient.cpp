#include <iostream>
#include <iterator>
#include "mysmtpclient.h"

#define LOG_PREFIX "My SMTP client: "

MySMTPClient::MySMTPClient(std::string address) : SMTPClient(address) {
	
}

void MySMTPClient::setLogin(const std::string& login) {
	this->login = login;
}

void MySMTPClient::setReceiver(const std::string& receiver) {
	this->receiver = receiver;
}

void MySMTPClient::setDataStream(std::istream& istream) {
	this->data = &istream;
}

void MySMTPClient::onHello() {
	std::cout << LOG_PREFIX << "Hello request succeeded. Sending mail from request" << std::endl;

	// Отправить запрос с отправителем
	SMTPRequest request;
	request.task = SMTPTasks::MAIL;
	request.payload = this->login.c_str();
	request.payload_size = this->login.size();
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onMail() {
	std::cout << LOG_PREFIX << "Mail from request succeeded. Sending recipient request" << std::endl;

	// Отправить запрос с отправителем
	SMTPRequest request;
	request.task = SMTPTasks::RCPT;
	request.payload = this->receiver.c_str();
	request.payload_size = this->receiver.size();
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onRecipient() {
	std::cout << LOG_PREFIX << "Recipient to request succeeded. Sending data request" << std::endl;

	// Отправить запрос на отправку данных
	SMTPRequest request;
	request.task = SMTPTasks::DATA;
	request.payload = nullptr;
	request.payload_size = 0;
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onData() {
	std::cout << LOG_PREFIX << "Data request succeeded. Sending a data" << std::endl;

	// Считать стрим в массив
	std::vector<char> data_raw(std::istreambuf_iterator<char>(*this->data),
		std::istreambuf_iterator<char>());

	// Отправить запрос на отправку данных
	SMTPRequest request;
	request.task = SMTPTasks::DATA_RAW;
	request.payload = &data_raw[0];
	request.payload_size = data_raw.size();
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onDataRaw() {
	std::cout << LOG_PREFIX << "Data sent. Now we can quit" << std::endl;

	// Отправить запрос на отправку данных
	SMTPRequest request;
	request.task = SMTPTasks::QUIT;
	request.payload = nullptr;
	request.payload_size = 0;
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onInit() {
	std::cout << LOG_PREFIX << "An initial connection established. Sending hello request" << std::endl;

	// Получить имя устройства
	char deviceName[1024];
	unsigned long deviceNameSize = sizeof(deviceName);
	if (!GetComputerNameA(deviceName, &deviceNameSize)) {
		std::cout << LOG_PREFIX << "Couldn't get computer name" << std::endl;
		this->shutdown();
	}

	// Отправить HELLO-запрос
	SMTPRequest request;
	request.task = SMTPTasks::HELO;
	request.payload = deviceName;
	request.payload_size = deviceNameSize;
	this->request((const char*)&request, sizeof(request));
}

void MySMTPClient::onQuit() {
	std::cout << LOG_PREFIX << "SMTP client quitted and says BB!" << std::endl;
	
	// Выключить клиент
	this->shutdown();
}

void MySMTPClient::onError(SMTPTasks::SMTPTasks failed_task, std::string response) {
	std::cout << LOG_PREFIX << "An error occured while send. Error: " << response << std::endl;

	// Выключить клиент
	this->shutdown();
}