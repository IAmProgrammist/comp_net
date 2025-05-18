#include "pch.h"

#include <iomanip>
#include <ctime>
#include <urlmon.h>
#include <filesystem>
#include <fstream>
#include <webstur/ip/tcp/http/httpresponse.h>

HTTPResponse::HTTPResponse(int code): code(code) {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    localtime_s(&tstruct, &now);

    //strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", &tstruct);
    strftime(buf, sizeof(buf), "%c", &tstruct);

    // Сохранить в заголовки сервер, дату и тип соединения
	this->headers.insert({ "Server", SERVER_NAME });
	this->headers.insert({ "Date", buf });
    this->headers.insert({ "Connection", "Closed" });
}

std::stringstream HTTPResponse::serialize() const {
    // Записать версию HTTP, код
    std::stringstream output;
    output << HTTP_VERSION << " " << this->code << "\r\n";
    // Записать заголовки
    for (auto& header : this->headers) {
        output << header.first << ": " << header.second << "\r\n";
    }
    output << "\r\n";
    return output;
}

HTTPResponse::~HTTPResponse() {

}

HTTPFileResponse::HTTPFileResponse(int code, std::string file_path) : HTTPResponse(code), file_path(file_path) {
    // Если файл существует
    std::ifstream in(this->file_path);
    if (in.is_open()) {
        // Считать содержимое файла, добавить MIME-тип и размер
        this->contents = std::string((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        in.close();
        this->headers.insert({ "Content-Length",
            std::to_string(this->contents.size()) });
        this->headers.insert({ "Content-Type", mimeTypeFromString(file_path) });
    }
    else {
        // Установить код в 404 - файла не существует
        this->code = 404;
    }

    // Закрыть файл
    in.close();
}

std::stringstream HTTPFileResponse::serialize() const {
    // Дозаписать в ответ содержимое файла
    auto response = HTTPResponse::serialize();
    response << this->contents;

    return response;
}

HTTPFileResponse::~HTTPFileResponse() {
}