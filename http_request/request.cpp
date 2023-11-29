
/*
 * To send a minimum HTTP request via terminal
 *
 * printf 'GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n' | nc www.google.com 80
 *
 * source: https://stackoverflow.com/questions/6686261/what-at-the-bare-minimum-is-required-for-an-http-request
*/

#include "request.hpp"

int parse_http_request_startline(Request &request, std::string startline)
{
	std::stringstream ss(startline);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	if (vstrings.size() != 3)
		return 1;
	request.startline["method"] = vstrings[0];
	request.startline["http_version"] = vstrings[2];
	//if (!(*(std::end(vstrings) - 1)).compare("\r\n"))
	//	return 1;
	return 0;
}

int parse_http_request(Request &request, std::string req)
{
	size_t last = 0, next = 0;
	std::string delim = "\r\n";
	std::string token;
	if ((next = req.find(delim, last)) != std::string::npos)
	{
		token = req.substr(last, next - last);
		if (parse_http_request_startline(request, token) == 1)
			std::cout << "syntax error 400" << std::endl;
		last = next + 2;
	}
	std::cout << request.startline["method"] << std::endl;
	std::cout << request.startline["http_version"] << std::endl;
	while ((next = req.find(delim, last)) != std::string::npos)
	{
		token = req.substr(last, next - last);
		last = next + 2;
	}
	//std::cout << req.substr(last);
	return 0;
}

int main()
{
	Request request;

	parse_http_request(request, "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n");
	
	return 0;
}
