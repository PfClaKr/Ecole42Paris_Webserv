#include "http_response/response.cpp"
#include "http_request/request.cpp"

int main()
{
	Request request;

	int status = parse_http_request(request, "GET / HTTP/1.\r\nHost: www.google.com\r\nConnection: close\r\n\r\n<html>\r\n\t<body></body>\r\n</html>\r\n");
	std::cout << "status: " << status << std::endl;
	http_response(request, status);
	print_http_request(request);
	return 0;
}
