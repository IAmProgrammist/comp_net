#include <fstream>
#include <webstur/utils.h>
#include <webstur/ip/tcp/tcpclient.h>

class DLLEXPORT FileTCPClient : public TCPClient {
	std::ofstream save_file;
	std::string save_path;
public:
	FileTCPClient(std::string address, 
				  int port = TCP_SERVER_DEFAULT_PORT,
				  std::string file_path = getUniqueFilepath());
protected:
	// Открывает файл для записи
	void onConnect();
	// Сохраняет полученный файл от сервера
	void onDisconnect();
	// Записывает данные от сервера в файл
	void onMessage(const std::vector<char>& message);
};