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

std::string Response::get_upload_file_name()
{
	return (this->upload_file_name);
}

void Response::set_header(std::string header, std::string value)
{
	this->header[header] = value;
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
		return false;
	}
	return true;
}

bool	Response::process_unchunk(Request &request)
{
	if (request.header.find("transfer-encoding") == request.header.end())
		return true;
	if (request.header["transfer-encoding"].compare("chunked") == 0)
	{
		std::cout << "inside chunked function\n";
		size_t next = 0, last = 0;
		std::string delim = "\r\n";
		std::string tmp, data;
		size_t len = 0;
		std::string result;
		while ((next = request.body.find(delim, last)) != std::string::npos)
		{
			tmp = request.body.substr(last, next - last);
			std::stringstream ss;
			ss << tmp;
			ss >> std::hex >> len >> std::dec;
			std::cout << std::dec;
			if (len == 0)
			{
				request.body = result;
				#ifdef DEBUG
					std::cout << RED << "============unchunk===============" << std::endl;
					std::cout << "result body : " << request.body << RESET <<std::endl;
				#endif
				return true;
			}
			last = next + delim.length();
			if ((next = request.body.find(delim, last)) == std::string::npos)
			{
				this->status_code = BAD_REQUEST;
				return false;
			}
			data = request.body.substr(last, next - last);
			last = next + delim.length();
			if (len != data.length())
			{
				this->status_code = BAD_REQUEST;
				return false;
			}
			result += data;
		}
		this->status_code = BAD_REQUEST;
		return false;
	}
	else
	{
		this->status_code = BAD_REQUEST;
		return false;
	}
	return true;
}

int	Response::check_body_size(Request &request, Context *context)
{
	int	content_length = std::atoi(request.header["content-length"].c_str());
	int max_body_size = std::atoi(context->get_directive_by_key("client_max_body_size")[0].c_str());
	int request_body_size = request.body.size();

	if (request_body_size > content_length)
	{
		this->status_code = PAYLOAD_TOO_LARGE;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with body_size big then Header in request integrity check" << RESET << std::endl;
		#endif
		return false;
	}
	if (request_body_size > max_body_size)
	{
		this->status_code = BAD_REQUEST;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with body_size big then config max size in request integrity check" << RESET << std::endl;
		#endif
		return false;
	}
	return true;
}

int	Response::check_method_allow(Request &request, Context *context)
{
	std::string request_method = request.startline["method"];
	std::vector<std::string> allowed_method = context->get_directive_by_key("allow_methods");

	for (unsigned long i = 0; i < allowed_method.size(); i++)
	{
		if (allowed_method[i] == request_method)
			return true;
	}
	this->status_code = METHOD_NOT_ALLOWED;
	#ifdef DEBUG
		std::cout << DARK_BLUE << "Error with not allowed in request integrity check" << RESET << std::endl;
	#endif
	return false;
}

int	Response::check_request_uri(Request &request, Context *context)
{
	(void) context;
	if (request.startline["uri"][0] != '/')
	{
		this->status_code = FORBIDDEN;
		#ifdef DEBUG
			std::cout << DARK_BLUE << "Error with uri in request integrity check" << RESET << std::endl;
		#endif
		return false;
	}
	return true;
}

int	Response::check_integrity_request(std::pair<Request, Context *> &response_set)
{
	if (!check_host_in_header(response_set.first)
		|| !check_body_size(response_set.first, response_set.second)
		|| !check_method_allow(response_set.first, response_set.second)
		|| !check_request_uri(response_set.first, response_set.second))
		return false;
	return true;
}

void Response::set_response_error_page()
{
	std::fstream file;
	std::string path = this->default_error_page[this->status_code];
	std::string raw_body;


	if (path[path.length() - 1] == '/')
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

void	Response::set_response(Request &request)
{
	std::ostringstream os;
	time_t clock = time(0);
	char *clock_to_char = ctime(&clock);
	std::string status_line = get_status_line(this->status_code);
	bool keep_alive = "keep-alive" == request.header["connection"] ? true : false;
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
	if (keep_alive)
		os << "Connection: keep-alive\r\n";
	else
		os << "Connection: close\r\n";
	if (default_error_page.find(status_code) != default_error_page.end() && status_code != 302)
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
		os << "\r\n" << body_cgi;
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
				std::cout << new_tmp << " : " << this->default_error_page[std::atoi(new_tmp.c_str())] << RESET << std::endl;
			#endif
		}
	}
}

void	Response::make_error_response(std::pair<Request, Context *> &response_set, int status_code)
{
	set_default_error_page(response_set.second);
	this->status_code = status_code;
	set_response(response_set.first);
}

bool	Response::process_request_body_if_multipart(Request &request)
{
	std::string type = request.header["content-type"];
	if (!type.empty() && type.find("multipart") != std::string::npos)
	{
		size_t pos = type.find("multipart/form-data");
		if (pos == std::string::npos)
		{
			this->status_code = UNSUPPORTED_MEDIA_TYPE;
			return false;
		}
		pos = type.find("boundary=") + 9; //length of boundary=
		std::string boundary = type.substr(pos, type.length() - pos);
		pos = request.body.find("\r\n");
		std::string first_line = request.body.substr(0, pos);
		request.body.erase(0, pos + 2);
		if (("--" + boundary) != first_line)
		{
			this->status_code = BAD_REQUEST;
			return false;
		}
		pos = request.body.find("filename=\"") + 10; //length of filename="
		std::string file_name = request.body.substr(pos, request.body.find("\"", pos) - pos);
		this->upload_file_name = file_name;
		pos = request.body.find("\r\n");
		request.body.erase(0, pos + 2);
		pos = request.body.find("\r\n");
		request.body.erase(0, pos + 2);
		request.body.erase(0, 2); //erase last \r\n
		pos = request.body.find("--" + boundary + "--");
		if (pos == std::string::npos)
		{
			this->status_code = BAD_REQUEST;
			return false;
		}
		request.body.erase(pos, request.body.length() - pos + 2);
		#ifdef DEBUG
			std::cout << DARK_BLUE << "===================body if multipart===================" << std::endl;
			std::cout << "boundary : " << boundary << std::endl;
			std::cout << "first line : " << first_line << std::endl;
			std::cout << "file name : " << file_name << RESET << std::endl;
		#endif
	}
	return true;
}

void	Response::make_http_response(std::pair<Request, Context *> &response_set)
{
	try
	{
		set_default_error_page(response_set.second);
		if (!process_request_body_if_multipart(response_set.first))
			return (set_response(response_set.first));
		if (!process_unchunk(response_set.first))
			return (set_response(response_set.first));
		if (!check_integrity_request(response_set))
			return (set_response(response_set.first));
		set_root_index_path(response_set.first, response_set.second);
		if (status_code > 0)
			return (set_response(response_set.first));
		handle_request_by_method(response_set.first, response_set.second);
		set_response(response_set.first);
	}
	catch (std::exception &e)
	{
		this->status_code = INTERNAL_SERVER_ERROR;
		set_response(response_set.first);
	}
	catch (Response::parsingException())
	{
		this->status_code = INTERNAL_SERVER_ERROR;
		set_response(response_set.first);
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
