#include <fstream>
#include <webstur/utils.h>
#include <webstur/ip/tcp/tcpclient.h>

class DLLEXPORT FileTCPClient : public TCPClient {
	std::ofstream save_file;
	std::string save_path, request_file;
public:
	FileTCPClient(std::string request_file,
				  std::string address, 
				  int port = TCP_SERVER_DEFAULT_PORT,
				  std::string file_path = getUniqueFilepath());
protected:
	// ��������� ���� ��� ������
	void onConnect();
	// ��������� ���������� ���� �� �������
	void onDisconnect();
	// ���������� ������ �� ������� � ����
	void onMessage(const std::vector<char>& message);
};