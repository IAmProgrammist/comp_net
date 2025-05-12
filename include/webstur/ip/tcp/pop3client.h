#pragma once

#include <queue>
#include <map>
#include <webstur/ip/tcp/tcpclient.h>
#include <sstream>

struct EMLHeader {
	std::size_t id;
	std::size_t size = 0;
};

struct EMLContent {
	std::string data;
};

namespace POP3Tasks {
	enum POP3Tasks
	{
		NONE,
		LIST,
		MAIL,
		DEL,
		INIT,
		LOGIN,
		PASSWORD,
		QUIT
	};
}

struct POP3Request {
	POP3Request() {};
	~POP3Request() {};
	POP3Tasks::POP3Tasks task;
	union Arguments {
		Arguments() {};
		~Arguments() {};

		std::size_t id;
		const char* login;
		const char* password;
	} arguments;
};

class DLLEXPORT POP3Client : public TCPClient {
private:
	POP3Tasks::POP3Tasks current_task;
	std::string buffer;

	std::string extractNextResponseFromBuffer();
	void delegateResponse(const std::string& response);

protected:
	static std::map<POP3Tasks::POP3Tasks, bool> is_multi_line;

	void onConnect();
	void onDisconnect();
	void onMessage(const std::vector<char>& message);
public:
	POP3Client(std::string address, int port = 110);

	void request(const char* payload, int payload_size);

	virtual void onMailList(std::vector<EMLHeader> mail_list) = 0;

	virtual void onMail(const EMLContent &mail_content) = 0;

	virtual void onMailDelete() = 0;

	virtual void onInit() = 0;

	virtual void onLogin() = 0;

	virtual void onPassword() = 0;

	virtual	void onQuit() = 0;

	virtual void onError(POP3Tasks::POP3Tasks failed_task, std::string response) = 0;
};