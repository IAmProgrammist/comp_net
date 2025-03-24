#pragma once

#include <string>
#include <WinSock2.h>
#include <vector>

#define SERVER_DEFAULT_PORT      0
#define CLIENT_DEFAULT_PORT      12484
#define IMAGE_FRAGMENT_SIZE      508
#define IMAGE_FRAGMENT_FILE_SIZE 512

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
// Сохраняет массив байтов в файл
void saveByteArray(const std::vector<char>& data, std::string path);