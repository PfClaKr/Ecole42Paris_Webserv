
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
	if (vstrings.size() != 3)
		return -1;
	request.startline["method"] = vstrings[0];
	if (vstrings[0].compare("GET") != 0 && vstrings[0].compare("POST") != 0 && vstrings[0].compare("DELETE") != 0)
		return -1;
	request.startline["uri"] = vstrings[1];
	request.startline["http_version"] = vstrings[2];
	if (vstrings[2].compare("HTTP/1.1") != 0)
		return -1;
	return 0;
}

int parse_https_request_header(Request &request, std::string headerline)
{
	size_t last = 0, next = 0;
	std::string delim = ":";
	std::string key, val;
	next = headerline.find(delim, last);
	if (next == std::string::npos)
		return -1;
	key = headerline.substr(last, next - last);
	transform(key.begin(), key.end(), key.begin(), ::tolower);
	last = next + 1;
	next = std::string::npos;
	val = headerline.substr(last, next);
	val.erase(0, val.find_first_not_of(" "));
	val.erase(val.find_last_not_of(" ") + 1);
	transform(val.begin(), val.end(), val.begin(), ::tolower);
	request.header[key] = val;
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
		if (parse_http_request_startline(request, token) == -1)
			return -1;
		last = next + 2;
	}
	while ((next = req.find(delim, last)) != std::string::npos)
	{
		token = req.substr(last, next - last);
		last = next + 2;
		if (token == "")
			break ;
		if (parse_https_request_header(request, token) == -1)
			return -1;
	}
	if (next != std::string::npos)
		request.body = req.substr(last, std::string::npos);
	return 0;
}

void print_http_request(Request &request)
{
	std::cout << DARK_BLUE;
	std::cout << "==============REQUEST_PARSING=================\n";
	std::cout << "Method : " << request.startline["method"] << std::endl;
	std::cout << "Uri    : " << request.startline["uri"] << std::endl;
	std::cout << "Version: " << request.startline["http_version"] << std::endl;
	std::cout << "===================HEADER=====================\n";
	for (std::map<std::string, std::string>::iterator it = request.header.begin(); it != request.header.end(); it++)
		std::cout << (*it).first << ": " << (*it).second << "\n";
	std::cout << "====================BODY======================\n";
	std::cout << request.body;
	std::cout << "==============================================\n" << RESET;
}

void	Request::clear_value()
{
	this->startline.clear();
	this->header.clear();
	this->body = "";
}

// int main()
// {
// 	Request request;

// 	int status = parse_http_request(request, "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: keep-alive\r\nsec-ch-ua: \"Not_A Brand\";v=\"8\", \"Chromium\";v=\"120\", \"Google Chrome\";v=\"120\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"Windows\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nSec-Fetch-Site: same-origin\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nReferer: http://127.0.0.1:8080/\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7,ru;q=0.6\r\n");
// 	std::cout << "status: " << status << std::endl;
// 	if (status == -1)
// 		return 1;
// 	print_http_request(request);
// 	return 0;
// }
