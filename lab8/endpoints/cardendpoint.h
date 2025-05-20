#pragma once

#include <webstur/ip/tcp/http/httpendpoint.h>

class CardEndpoint : public HTTPEndpoint {
public:
	CardEndpoint();

	HTTPResponse* process(const HTTPRequest& request);
};