#include "pch.h"
#include <webstur/ip/tcp/http/httprequest.h>
#include <webstur/ip/tcp/http/httpendpoint.h>

bool HTTPEndpoint::matches(const HTTPRequest& message) {
	return (message.getMethod() == HEAD || message.getMethod() == this->method)
		&& message.matchesPath(this->path);
}

HTTPEndpoint::HTTPEndpoint(HTTPEndpoint& endpoint) {
	this->path = endpoint.path;
	this->method = endpoint.method;
}