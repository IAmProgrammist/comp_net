#include "pch.h"

#include <iostream>
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpserver.h>

HTTPServer::HTTPServer(int port) : TCPServer(port) {};

void HTTPServer::injectEndpoint(HTTPEndpoint& endpoint) {
	this->endpoints.push_back(&endpoint);
}

void HTTPServer::onConnect(SOCKET client) {
	std::clog << "A new HTTP server connection established" << std::endl;
	this->buffers.insert(std::pair{client, ""});
	this->content_lengths.insert({ client, std::string::npos });
	this->header_end_poses.insert({ client, std::string::npos });
}

void HTTPServer::onDisconnect(SOCKET client) {
	std::clog << "An HTTP connection is ended" << std::endl;
	this->buffers.erase(this->buffers.find(client));
	this->content_lengths.erase(this->content_lengths.find(client));
	this->header_end_poses.erase(this->header_end_poses.find(client));
}

void HTTPServer::onMessage(SOCKET client, const std::vector<char>& message) {
	std::string& buffer = this->buffers.find(client)->second;
	std::size_t& content_length = this->content_lengths.find(client)->second;
	std::size_t& header_end_pos = this->header_end_poses.find(client)->second;

	buffer += std::string(message.begin(), message.end());

	if (header_end_pos == std::string::npos) {
		header_end_pos = buffer.find("\r\n\r\n");

		if (header_end_pos == std::string::npos)
			return;

		auto opt_parsed_header = HTTPRequest::parse(buffer.substr(0, header_end_pos + 4));
		if (!opt_parsed_header.isValid()) {
			HTTPResponse response_422 = HTTPResponse(422);
			sendMessageAndDisconnect(client, response_422);
			return;
		}

		auto headers = opt_parsed_header.getHeaders();
		auto content_length_it = headers.find("Content-Length");

		if (content_length_it != headers.end()) {
			content_length = std::atoi(content_length_it->second.c_str());
		}
		else {
			onRequest(client, opt_parsed_header);
		}
	}
	else {
		if (buffer.size() - header_end_pos + 4 >= content_length) {
			auto request = HTTPRequest::parse(buffer.substr(0, header_end_pos));

			onRequest(client, request);
		}
	}
}

void HTTPServer::onRequest(SOCKET client, HTTPRequest& message) {
	HTTPEndpoint* endpoint = nullptr;
	
	for (auto& p_endpoint : this->endpoints) {
		if (p_endpoint->matches(message)) {
			endpoint = p_endpoint;
			break;
		}
	}

	if (endpoint == nullptr) {
		HTTPResponse response_404 = HTTPResponse(404);
		sendMessageAndDisconnect(client, response_404);
		return;
	}

	if (message.getMethod() == HEAD) {
		HTTPResponse response_200 = HTTPResponse(200);
		sendMessageAndDisconnect(client, response_200);
		return;
	}

	auto response = endpoint->process(message);
	sendMessageAndDisconnect(client, *response);
	delete response;
}

void HTTPServer::sendMessageAndDisconnect(SOCKET client, const HTTPResponse& response) {
	auto response_stream = response.serialize();
	this->sendMessage(client, response_stream);
	this->disconnect(client);
}