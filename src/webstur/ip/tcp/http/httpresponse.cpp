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


	this->headers.insert({ "Server", SERVER_NAME });
	this->headers.insert({ "Date", buf });
    this->headers.insert({ "Connection", "Closed" });
}

std::stringstream HTTPResponse::serialize() const {
    std::stringstream output;
    output << HTTP_VERSION << " " << this->code << "\r\n";
    for (auto& header : this->headers) {
        output << header.first << ": " << header.second << "\r\n";
    }
    output << "\r\n";
    return output;
}

HTTPResponse::~HTTPResponse() {

}

HTTPFileResponse::HTTPFileResponse(int code, std::string file_path) : HTTPResponse(code), file_path(file_path) {
    std::ifstream in(this->file_path);
    if (in.is_open()) {
        this->contents = std::string((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        in.close();
        this->headers.insert({ "Content-Length",
            std::to_string(this->contents.size()) });
        this->headers.insert({ "Content-Type", mimeTypeFromString(file_path) });
    }
    else {
        this->code = 404;
    }
    in.close();
}

std::stringstream HTTPFileResponse::serialize() const {
    auto response = HTTPResponse::serialize();
    response << this->contents;

    return response;
}

HTTPFileResponse::~HTTPFileResponse() {
}