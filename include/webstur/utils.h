#pragma once

#ifdef WEBSTUR_EXPORTS
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

#define FUNC_DLLEXPORT DLLEXPORT

#include <string>
#include <WinSock2.h>
#include <vector>

#define UDP_SERVER_DEFAULT_PORT       0
#define UDP_CLIENT_DEFAULT_PORT       12484
#define UDP_IMAGE_FRAGMENT_SIZE       508
#define IMAGE_FRAGMENT_FILE_SIZE      512
#define TCP_SERVER_DEFAULT_PORT       3178
#define TCP_MAX_WORKER_THREADS_AMOUNT 256
#define TCP_SERVER_TIMEOUT_S          10
#define TCP_MAX_MESSAGE_SIZE          16000

// Формирует текст ошибки вместе с WSA кодом
FUNC_DLLEXPORT std::string getErrorTextWithWSAErrorCode(std::string errorText);
// Выводит информацию о структуре sockaddr
FUNC_DLLEXPORT std::ostream& printSockaddrInfo(std::ostream& out, sockaddr_in& sock);
// Возвращает IP адрес для текущего ПК
FUNC_DLLEXPORT sockaddr_in getDeviceAddrInfo();
// Загружает WSA
FUNC_DLLEXPORT void loadWSA();
// Выгружает WSA
FUNC_DLLEXPORT void unloadWSA();
// Возвращает неконфликтующее имя для текущего файла
FUNC_DLLEXPORT std::string getUniqueFilepath();
// Сохраняет массив байтов в файл
FUNC_DLLEXPORT void saveByteArray(const std::vector<char>& data, std::string path);
// Разбивает строку на массив строк по разделителю
FUNC_DLLEXPORT std::vector<std::string> split(std::string s, std::string delimiter);
// Получает MIME-тип для файла по пути str
FUNC_DLLEXPORT std::string mimeTypeFromString(const std::string& str);