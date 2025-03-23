#pragma once

#include <string>
#include <WinSock2.h>

#define SERVER_DEFAULT_PORT 0
#define CLIENT_DEFAULT_PORT 12484
#define IMAGE_FRAGMENT_SIZE 508

// Формирует текст ошибки вместе с WSA кодом
std::string getErrorTextWithWSAErrorCode(std::string errorText);
// Выводит информацию о структуре sockaddr
std::ostream& printSockaddrInfo(std::ostream& out, sockaddr_in& sock);
// Возвращает IP адрес для текущего ПК
sockaddr_in getDeviceAddrInfo();
// Загружает WSA
void loadWSA();
// Выгружает WSA
void unloadWSA();
// Возвращает неконфликтующее имя для текущего файла
std::string getUniqueFilepath();