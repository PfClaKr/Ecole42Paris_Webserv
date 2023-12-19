#include "response.hpp"

std::string Response::get_query()
{
	return (this->query);
}

std::string Response::get_path()
{
	return (this->path);
}

std::string Response::get_ready_to_send()
{
	return (this->ready_to_send);
}

int	Response::check_host_in_header(Request &request)
{
	std::string host = request.header["host"];
	if (host.empty())
	{
		this->status_code = BAD_REQUEST;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with no host in request integrity check" << RESET << std::endl;
		#endif
		return -1;
	}
	return 0;
}

int	Response::check_body_size(Request &request, Context *context)
{
	int	content_length = std::atoi(request.header["content_length"].c_str());
	int max_body_size = std::atoi(context->get_directive_by_key("client_max_body_size")[0].c_str());
	int request_body_size = request.body.size();

	if (request_body_size > content_length)
	{
		this->status_code = PAYLOAD_TOO_LARGE;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with body_size big then Header in request integrity check" << RESET << std::endl;
		#endif
		return -1;
	}
	if (request_body_size > max_body_size)
	{
		this->status_code = BAD_REQUEST;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with body_size big then config max size in request integrity check" << RESET << std::endl;
		#endif
		return -1;
	}
	return 0;
}

int	Response::check_method_allow(Request &request, Context *context)
{
	std::string request_method = request.startline["method"];
	std::vector<std::string> allowed_method = context->get_directive_by_key("allow_methods");

	for (unsigned long i = 0; i < allowed_method.size(); i++)
	{
		if (allowed_method[i] == request_method)
			return 0;
	}
	this->status_code = METHOD_NOT_ALLOWED;
	#ifdef DEBUG
		std::cout << DARK_BLUE << "Error with not allowed in request integrity check" << RESET << std::endl;
	#endif
	return -1;
}

int	Response::check_request_uri(Request &request, Context *context)
{
	(void) context;
	if (request.startline["uri"].front() != '/')
	{
		this->status_code = FORBIDDEN;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with uri in request integrity check" << RESET << std::endl;
		#endif
		return -1;
	}
	return 0;
}

int	Response::check_integrity_request(std::pair<Request, Context *> &response_set)
{
	if (!check_host_in_header(response_set.first)
		|| !check_body_size(response_set.first, response_set.second)
		|| !check_method_allow(response_set.first, response_set.second)
		|| !check_request_uri(response_set.first, response_set.second))
		return -1;
	return 0;
}

void Response::set_response_error_page()
{
	std::fstream file;
	std::string path = this->default_error_page[this->status_code];
	std::string raw_body;


	if (path.back() == '/')
		return ;
	file.open(path.c_str(), std::ios::in);
	if (!file.good())
		return ;
	raw_body = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
	this->body.first = raw_body;
	this->body.second = "text/html";
	#ifdef DEBUG
		std::cout << DARK_BLUE << "===========Set response error page============" << std::endl;
		std::cout << "Path -> " << path << RESET << std::endl;
	#endif
}

void	Response::set_response()
{
	std::ostringstream os;
	time_t clock = time(0);
	char *clock_to_char = ctime(&clock);
	std::string status_line = get_status_line(this->status_code);
	#ifdef DEBUG
		std::cout << DARK_BLUE << "==============Set response===============" << std::endl;
		std::cout << "Status code : " << status_code << std::endl;
		std::cout << "is body empty ? : " << this->body.first.empty() << std::endl;
		std::cout << RESET;
	#endif
	if (status_line.empty())
	{
		this->status_code = 500;
		status_line = get_status_line(500);
	}

	os << "HTTP/1.1 " << this->status_code << " " << status_line << "\r\n";
	os << "Accept-Ranges: bytes\r\n";
	os << "Server: ychunschae's engine\r\n";
	os << "Date: " << clock_to_char;
	for (std::map<std::string, std::string>::iterator it = this->header.begin(); it != this->header.end(); it++)
		os << it->first << ": " << it->second << "\r\n";
	if (header.find("Connection") == header.end())
		os << "Connection: keep-alive\r\n";
	if (default_error_page.find(status_code) != default_error_page.end() && !body.first.empty())
		set_response_error_page();
	else if (status_code >= 300)
	{
		body.first = "<h1>" + Response::get_status_line(status_code) + "</h1>";
	}
	if (body.first.size())
	{
		os << "Content-Length: " << body.first.size() << "\r\n";
		os << "Content-Type: " << body.second << "\r\n\r\n";
		os << body.first;
	}
	else if (body_cgi.size())
		os << body_cgi;
	else
	{
		os << "Content-Length: 0\r\n";
		os << "Content-Type: text/html\r\n\r\n" << std::endl;
	}
	this->ready_to_send = os.str();
}

void	Response::set_default_error_page(Context *context)
{
	const Context *http = context->get_parent();
	std::map<std::string, std::vector<std::string> > directive = http->get_directive();
	std::string root = http->get_directive_by_key("root").front();

	for (std::map<std::string, std::vector<std::string> >::iterator it = directive.begin(); it != directive.end(); it++)
	{
		std::pair<std::string, std::vector<std::string> > tmp = *it;
		if (tmp.first.find("default_error_page") != std::string::npos)
		{
			const unsigned long pos = tmp.first.find_first_not_of("default_error_page");
			if (pos == std::string::npos || pos != tmp.first.size() - 3)
				continue;
			std::string new_tmp = tmp.first.substr(pos, tmp.first.size());
			this->default_error_page[std::atoi(new_tmp.c_str())] = root + tmp.second.front();
			#ifdef DEBUG
				std::cout << DARK_BLUE << "==============Set default error page============" << std::endl;
				std::cout << "Raw was :" << tmp.first << " " << tmp.second.front() << std::endl;
				std::cout << new_tmp << " : " << tmp.second.front() << RESET << std::endl;
			#endif
		}
	}
}

void	Response::make_error_response(Response &response, std::pair<Request, Context *> &response_set, int status_code)
{
	(void) response;
	set_default_error_page(response_set.second);
	this->status_code = status_code;
	set_response();
}

void	Response::make_http_response(Response &response, std::pair<Request, Context *> &response_set)
{
	(void) response;
	try
	{
		set_default_error_page(response_set.second);
		if (!check_integrity_request(response_set))
			return (set_response());
		set_root_index_path(response_set.first, response_set.second);
		if (status_code > 0)
			return (set_response());
		handle_request_by_method(response_set.first, response_set.second);
		set_response();
	}
	catch (std::exception &e)
	{
		this->status_code = INTERNAL_SERVER_ERROR;
		set_response();
	}
	catch (Response::parsingException())
	{
		this->status_code = INTERNAL_SERVER_ERROR;
		set_response();
	}
}

Response::Response()
{
	status_code = -1;
}

Response::Response(const Response &ref)
{
	(void)ref;
}

Response::~Response()
{
}
