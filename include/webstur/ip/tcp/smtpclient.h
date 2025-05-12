#pragma once

#include <queue>
#include <map>
#include <webstur/ip/tcp/tcpclient.h>
#include <sstream>

namespace SMTPTasks {
	enum SMTPTasks
	{
		NONE,
		INIT,
		HELO,
		MAIL,
		RCPT,
		DATA,
		DATA_RAW,
		QUIT
	};
}

struct SMTPRequest {
	SMTPRequest() {};
	~SMTPRequest() {};
	
	SMTPTasks::SMTPTasks task = SMTPTasks::NONE;
	const char* payload = nullptr;
	std::size_t payload_size = 0;
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