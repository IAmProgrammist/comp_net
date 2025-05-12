// Содержит базовые классы для создания SMTP-клиента

#pragma once

#include <queue>
#include <map>
#include <webstur/ip/tcp/tcpclient.h>
#include <sstream>

namespace SMTPTasks {
	// Структура, описывающая типы задач, выполняемые SMTP-клиентом
	enum SMTPTasks
	{
		// У клиента нет задачи, все входящие сообщения будут проигнорированы
		NONE,
		// Первое сообщение от сервера, подтверждающее успешное соединение
		INIT,
		// Отправка информации о комьютере
		HELO,
		// Отправка информации об отправителе
		MAIL,
		// Отправка информации о получателе
		RCPT,
		// Отправка запроса на приём почты
		DATA,
		// Отправка тела письма
		DATA_RAW,
		// Выход из SMTP-сессии
		QUIT
	};
}

// Описывает задачу к выполнению и её аргументы
struct SMTPRequest {
	SMTPRequest() {};
	~SMTPRequest() {};
	
	SMTPTasks::SMTPTasks task = SMTPTasks::NONE;
	union Arguments {
		Arguments() {};
		~Arguments() {};

		// Имя компьютера, используется в задаче INIT
		const char* helo;
		// Отправитель, используется в задаче MAIL
		const char* from;
		// Отправитель, используется в задаче RCPT
		const char* to;
		// Письмо, используется в задаче DATA_RAW
		struct MailContents {
			MailContents() {};
			~MailContents() {};

			// Тело письма
			const char* body;
			// Размер письма
			std::size_t size;
		} mail;
	} arguments;
};

class DLLEXPORT SMTPClient : public TCPClient {
private:
	SMTPTasks::SMTPTasks current_task;
	std::string buffer;

	std::string extractNextResponseFromBuffer();
	void delegateResponse(const std::string& response);

protected:
	void onConnect();
	void onDisconnect();
	void onMessage(const std::vector<char>& message);
public:
	SMTPClient(std::string address, int port = 25);

	void request(const char* payload, int payload_size);

	virtual void onHello() = 0;

	virtual void onMail() = 0;

	virtual void onRecipient() = 0;

	virtual void onData() = 0;

	virtual void onDataRaw() = 0;

	virtual void onInit() = 0;

	virtual	void onQuit() = 0;

	virtual void onError(SMTPTasks::SMTPTasks failed_task, std::string response) = 0;
};