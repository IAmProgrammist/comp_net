#pragma once

#include <vector>
#include "iclient.h"

class Client : public IClient {
protected:
	SOCKET socket_descriptor;
	std::vector<char> temporary_data;
public:
	// Создаёт клиента
	Client();
	// Освобождает ресурсы клиента
	virtual ~Client();
	// Запрашивает данные с клиента с дополнительной отправкой данных.
	// payload и payload_size игнорируется в данной реализации
	void request(char* payload, int payload_size);
	// Сохраняет данные полученные от сервера
	std::string save();
	// Отобразить информацию о клиенте
	std::ostream& printClientInfo(std::ostream& out);
};