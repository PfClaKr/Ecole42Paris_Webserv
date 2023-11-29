
/*
 * To send a minimum HTTP request via terminal
 *
 * printf 'GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n' | nc www.google.com 80
 *
 * source: https://stackoverflow.com/questions/6686261/what-at-the-bare-minimum-is-required-for-an-http-request
*/

/*
 * About invalid HTTP request startline 
 * https://stackoverflow.com/questions/7512823/c-insert-into-stdmap-without-knowing-a-key
*/

#include "request.hpp"

int parse_http_request_startline(Request &request, std::string startline)
{
	std::stringstream ss(startline);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	if (vstrings.size() < 1 || vstrings.size() > 3)
		return 1; // still has to send a bad request = webserver is the one who decides about integrity
	request.startline["method"] = "GET";
	request.startline["uri"] = "/"; // don't know yet what is the default value
	request.startline["http_version"] = "HTTP/1.0"; // by default
	switch (vstrings.size())
	{
		case 3:
			request.startline["http_version"] = vstrings[2];
			// fall through
		case 2:
			request.startline["uri"] = vstrings[1]; // how to detect if it's 'http version' or 'uri'
			// fall through
		case 1:
			request.startline["method"] = vstrings[0];
			// fall through
	}
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
			return 400;
		last = next + 2;
	}
	while ((next = req.find(delim, last)) != std::string::npos)
	{
		token = req.substr(last, next - last);
		last = next + 2;
		if (token == "") // double CRLF '\r\n\r\n' = the end of header
			break ;
		request.header.push_back(token); // split into key:value format on response side
	}
	request.body = req.substr(last, std::string::npos);
	return 0;
}

void print_http_request(Request &request)
{
	std::cout << "=============================================\n";
	std::cout << "Method : " << request.startline["method"] << std::endl;
	std::cout << "Uri    : " << request.startline["uri"] << std::endl;
	std::cout << "Version: " << request.startline["http_version"] << std::endl;
	std::cout << "=============================================\n";
	for (int i = 0; i < request.header.size(); i++)
	{
		std::cout << "Header : " << request.header[i] << std::endl;
	}
	std::cout << "=============================================\n";
	std::cout << "Body   :\n" << request.body;
	std::cout << "=============================================\n";
}

int main()
{
	Request request;

	parse_http_request(request, "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n<html>\r\n\t<body></body>\r\n</html>\r\n");
	print_http_request(request);
	return 0;
}
