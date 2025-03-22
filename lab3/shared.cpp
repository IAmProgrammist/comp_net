#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <filesystem>
#include <random>
#include <sstream>
#include "shared.h"

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}


std::string getErrorTextWithWSAErrorCode(std::string errorText) {
	std::ostringstream resultError;
	resultError << errorText << " " << WSAGetLastError();

	return resultError.str();
}

std::ostream& printSockaddrInfo(std::ostream& out, sockaddr_in& sock) {
	char address[64] = {};
	inet_ntop(AF_INET, &sock.sin_addr, address, sizeof(address));

	out << "  Address: " << address << "\n" <<
		"  Port: " << htons(sock.sin_port);

	return out;
}

sockaddr_in getDeviceAddrInfo() {
    // Получить имя текущего устройства
    char host_name[256] = {};
    if (gethostname(host_name, sizeof(host_name)) == SOCKET_ERROR)
        throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to get host name"));

    addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    struct addrinfo* result = NULL;

    // Получить информацию об адресах на устройстве в сети
    DWORD dwRetval = getaddrinfo(host_name, "", &hints, &result);
    if (dwRetval != 0) {
        throw std::runtime_error(getErrorTextWithWSAErrorCode("Getaddrinfo failed for device host name"));
    }

    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // Если адрес для устройства является IPv4 адресом, мы нашли нужный адрес, возвращаем его
        if (ptr->ai_family == AF_INET) {
            sockaddr_in device_sockaddr = {};
            memcpy(&device_sockaddr, ptr->ai_addr, sizeof(device_sockaddr));

            return device_sockaddr;
        }
    }
    
    throw std::runtime_error(getErrorTextWithWSAErrorCode("Getaddrinfo failed for device host name"));
}

void loadWSA() {
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 0);

    // Загрузить библиотеку WinSock
    if (WSAStartup(wVersionRequested, &wsaData) == SOCKET_ERROR)
        throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to load WSA library"));
}

void unloadWSA() {
    // Выгрузить библиотеку WinSock
    if (WSACleanup() == SOCKET_ERROR)
        throw std::runtime_error(getErrorTextWithWSAErrorCode("Unable to unload WSA library"));
}

std::string getUniqueFilepath() {
    return "./" + uuid::generate_uuid_v4() + ".jpg";
}