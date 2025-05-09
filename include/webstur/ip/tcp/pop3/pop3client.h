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

enum POP3Tasks
{
	NONE,
	LIST,
	MAIL,
	DEL,
	INIT
};

class DLLEXPORT POP3Client : protected TCPClient {
private:
	POP3Tasks current_task;
	std::string buffer;

protected:
	static std::map<POP3Tasks, bool> is_multi_line;

	void onConnect();
	void onDisconnect();
	void onMessage(const std::vector<char>& message);
public:
	POP3Client(std::string address, int port, std::string login, std::string password);

	void requestMailList();

	void requestMail(EMLHeader header);

	void requestMailDelete(EMLHeader header);

	void onMailList(std::vector<EMLHeader> mail_list);

	void onMail(const EMLContent &mail_content);

	void onMailDelete();

	void onInit();

	void onError(POP3Tasks failed_task, std::string response);
};