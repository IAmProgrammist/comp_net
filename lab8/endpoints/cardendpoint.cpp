#include "cardendpoint.h"

// ��������������� �������� �� ���� /my-card
CardEndpoint::CardEndpoint() : HTTPEndpoint("/my-card", GET) {};

HTTPResponse* CardEndpoint::process(const HTTPRequest& request) {
	// ������ ����-�������
	HTTPResponse* answer = new HTTPFileResponse(200, ".\\assets\\index.html");

	return answer;
}