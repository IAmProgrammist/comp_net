#include "pch.h"
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <random>
#include <fstream>
#include <string>
#include <urlmon.h>
#include <webstur/utils.h>

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
			// Плохой костыль, нужно было использовать GetAdaptersInfo
			// Иначе не получится найти нужный айпишник с гейтвеем на роутер
			if (device_sockaddr.sin_addr.S_un.S_un_b.s_b3 == 56) {
				continue;
			}
			
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

void saveByteArray(const std::vector<char>& data, std::string path) {
	// Открываем файл
	std::ofstream save_file(path, std::ios::binary);

	if (!save_file.good())
		throw std::runtime_error("Unable to open file '" + path + "' for reading");

	// Пишем массив в файл
	for (unsigned i = 0; i < data.size() / IMAGE_FRAGMENT_FILE_SIZE; i++)
		save_file.write(&data[i * IMAGE_FRAGMENT_FILE_SIZE], sizeof(char) * IMAGE_FRAGMENT_FILE_SIZE);

	// Сохраняем и закрываем файл
	save_file.flush();
	save_file.close();
}

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

std::string mimeTypeFromString(const std::string& str) {
	LPWSTR pwzMimeOut = NULL;
	HRESULT hr = FindMimeFromData(NULL, std::wstring(str.begin(), str.end()).c_str(), NULL, 0,
		NULL, FMFD_URLASFILENAME, &pwzMimeOut, 0x0);
	if (SUCCEEDED(hr)) {
		std::wstring strResult(pwzMimeOut);
		// Despite the documentation stating to call operator delete, the
		// returned string must be cleaned up using CoTaskMemFree
		CoTaskMemFree(pwzMimeOut);
		return std::string(strResult.begin(), strResult.end());
	}

	return "";

}