// Содержит базовые классы для создания POP3-клиента

#pragma once

#include <queue>
#include <map>
#include <webstur/ip/tcp/tcpclient.h>
#include <sstream>

// Структура, описывающая идентификатор и размер письма
struct EMLHeader {
	std::size_t id;
	std::size_t size = 0;
};

// Структура, содержащая данные полученного письма
struct EMLContent {
	std::string data;
};

namespace POP3Tasks {
	// Структура, описывающая типы задач, выполняемые POP3-клиентом
	enum POP3Tasks
	{
		// У клиента нет задачи, все входящие сообщения будут проигнорированы
		NONE,
		// Список непрочитанных сообщений
		LIST,
		// Получение конкретного сообщения
		MAIL,
		// Удалить сообщение
		DEL,
		// Первое сообщение от сервера +OK
		INIT,
		// Ввод логина от почтового ящика
		LOGIN,
		// Ввод пароля от почтового ящика
		PASSWORD,
		// Выход из POP3-сессии
		QUIT
	};
}

// Описывает задачу к выполнению и её аргументы
struct POP3Request {
	POP3Request() {};
	~POP3Request() {};
	// Тип задачи
	POP3Tasks::POP3Tasks task;
	union Arguments {
		Arguments() {};
		~Arguments() {};

		// Идентификатор сообщения, используется в задачах DEL, MAIL
		std::size_t id;
		// Логин, используется в задаче LOGIN
		const char* login;
		// Пароль, используется в задаче PASSWORD
		const char* password;
	} arguments;
};

// Абстрактный POP3-клиент, исользующий незащищённое соединение. 
// Отправляет команды и принимает ответы от сервера. 
// Вызывает соответствующий метод для каждого ответа.
class DLLEXPORT POP3Client : public TCPClient {
private:
	// Текущая активная задача
	POP3Tasks::POP3Tasks current_task;
	// Буфер накопления ответов с сервера
	std::string buffer;
	// Извлекает из буфера следующий ответ от сервера
	std::string extractNextResponseFromBuffer();
	// Распарсивает ответ от сервера, распределяет в соответствующий
	// метод в зависимости от текущей задачи, сбрасывает текущую задачу.
	void delegateResponse(const std::string& response);
protected:
	// Определяет, является ли команда мультистроковой.
	static std::map<POP3Tasks::POP3Tasks, bool> is_multi_line;
	// Метод, вызываемый при установлении соединения с сервером
	void onConnect();
	// Метод, вызываемый при разрыве соединения с сервером
	// Переданный сокет уже не является действительным, 
	// однако передаётся для отладочной информации
	void onDisconnect();
	// Метод, вызываемый при отправке получении сообщения от сервера
	void onMessage(const std::vector<char>& message);
public:
	// Конструирует POP3-клиент, порт по умолчанию - 110 (незащищённое соединение)
	POP3Client(std::string address, int port = 110);
	// Метод для отправки команды серверу. 
	// В параметр payload передаётся указатель на структуру POP3Request,
	// в payload_size - размер структуры
	void request(const char* payload, int payload_size);
	// Метод, вызываемый при получении списка доступных писем
	virtual void onMailList(std::vector<EMLHeader> mail_list) = 0;
	// Метод, вызываемый при получении письма
	virtual void onMail(const EMLContent &mail_content) = 0;
	// Метод, вызываемый после удаления письма
	virtual void onMailDelete() = 0;
	// Метод, вызываемый после успешного подтверждения
	// соединения с сервером
	virtual void onInit() = 0;
	// Метод, вызываемый после успешного ввода логина
	virtual void onLogin() = 0;
	// Метод, вызываемый после успешного ввода пароля
	virtual void onPassword() = 0;
	// Метод, вызываемый после получения ответа на выход из POP3-сессии
	virtual	void onQuit() = 0;
	// Метод, вызываемый если не удалось обработать ответ от сервера
	virtual void onError(POP3Tasks::POP3Tasks failed_task, std::string response) = 0;
};