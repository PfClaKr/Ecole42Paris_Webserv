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
	std::string host = request.header["Host"];
	if (host.empty())
	{
		this->status_code = BAD_REQUEST;
		return -1;
	}
	return 0;
}

int	Response::check_body_size(Request &request, Context *context)
{
	int	content_length = std::atoi(request.header["Contetn_length"].c_str());
	int max_body_size = std::atoi(context->get_directive_by_key("client_max_body_size")[0].c_str());
	int request_body_size = request.body.size();

	if (request_body_size > content_length)
	{
		this->status_code = PAYLOAD_TOO_LARGE;
		return -1;
	}
	if (request_body_size > max_body_size)
	{
		this->status_code = BAD_REQUEST;
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
	return -1;
}

int	Response::check_request_uri(Request &request, Context *context)
{
	(void) context;
	if (request.startline["uri"].front() != '/')
	{
		this->status_code = FORBIDDEN;
		return -1;
	}
	return 0;
}

void	Response::check_index_or_redirection(Request &request, Context *context)
{
	std::pair<std::string, std::vector<std::string> > args = *(context->get_directive().begin());

	if (args.first == "return")
	{
		if (request.startline["method"] == "DELETE")
		{
			this->status_code = BAD_REQUEST;
			return ;
		}
		this->status_code = MOVED_PERMANENTLY;
		this->header["Location"] = args.second.front();
		return ;
	}
	else if (args.first == "index")
	{
		std::string location_path = context->get_args().front();
		if (location_path.back() != '/')
			location_path += '/';
		this->path += location_path + args.second.front();
	}
}

bool	Response::set_location_in_request(Request &request, Context *context)
{
	std::vector<Context *> location = context->get_child();
	std::string path = request.startline["uri"];

	for (unsigned long i = 0; i < location.size(); i++)
	{
		if (path == location[i]->get_args()[0] && "location" == location[i]->get_name())
		{
			check_index_or_redirection(request, location[i]);
			return (true);
		}
	}
	return (false);
}

void	Response::set_root_index_path(Request &request, Context *context)
{
	const Context *http = context->get_parent();
	std::string root = http->get_directive_by_key("root").front();
	std::string request_path = request.startline["uri"];
	size_t pos_query;

	if (root.back() != '/')
		root += '/';
	pos_query = request_path.find("?");
	if (pos_query != std::string::npos)
	{
		this->path = root + request_path.substr(0, pos_query);
		this->query = request_path.substr(pos_query + 1, request_path.size());
	}
	this->path = root;
	if (set_location_in_request(request, context))		;
	else
		this->path += request_path;
	this->query = "";
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

bool	check_is_php(std::string file)
{
	std::string extension;
	for (std::string::reverse_iterator it = file.rbegin(); it != file.rend(); it++)
	{
		if (*it == '.')
			break;
		extension += *it;
	}
	if (extension == "php")
		return true;
	return false;
}

void	Response::directory_autoindex()
{
	DIR *dir;
	struct dirent *dir_content;
	struct stat fileinfo;
	std::string body_tmp;

	if (!(dir = opendir(this->path.c_str())))
	{
		this->status_code = NOT_FOUND;
		return ;
	}
	else
	{
		body_tmp += "<h1>Index of " + this->path + "<h1>\n";
		while ((dir_content = readdir(dir)) != NULL)
		{
			std::string tmp = std::string(dir_content->d_name);
			if (tmp != ".")
			{
				if (stat((this->path + "/" + tmp).c_str(), &fileinfo) == 0)
				{
					if (S_ISDIR(fileinfo.st_mode))
					{
						tmp += "/";
						body_tmp += "\t\t<a href=\"" + this->path + tmp + "\">" + tmp + "</a>";
					}
					else
						body_tmp += "\t\t<a href=\"" + this->path + tmp + "\">" + tmp + "</a>";
				}
			}
		}
	}
	closedir(dir);
	body.first = body_tmp;
	body.second = "text/html";
}

void	Response::handle_get_method(Request &request, Context *context)
{
	Cgi cgi;
	std::fstream file;
	bool is_dir = this->path.back() == '/';
	bool is_php = check_is_php(this->path);
	const Context *http = context->get_parent();
	std::string autoindex = http->get_directive_by_key("autoindex").front();
	bool is_autoindex = autoindex == "on" ? false : true;

	if (!request.body.empty())
	{
		this->status_code = BAD_REQUEST;
		return ;
	}

	file.open(this->path.c_str(), std::ios::in);
	if (file.good() && is_dir && is_autoindex) //path is directory
		directory_autoindex();
	else if (!is_dir)
	{
		this->status_code = NOT_FOUND;
		file.close();	
		return ;
	}
	else if (is_php)
	{
		try
		{
			body_cgi = cgi.init_cgi(request, context, this);
		}
		catch (Cgi::CgiException())
		{
			this->status_code = INTERNAL_SERVER_ERROR;
			file.close();
			return ;
		}
		header["Content-Length"] = body_cgi.size();
		body.first.clear();
		body.second.clear();
	}
	else
	{
		body.first = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		body.second = get_mime_type(this->path);
	}
	file.close();
	this->status_code = OK;
}

void	Response::handle_post_method(Request &request, Context *context)
{
	Cgi cgi;
	std::fstream file;
	bool is_dir = this->path.back() == '/';

	file.open(this->path.c_str(), std::ios::in);
	if (is_dir)
	{
		this->status_code = METHOD_NOT_ALLOWED;
		file.close();
		return ;
	}
	if (file.good())
	{
		try
		{
			body_cgi = cgi.init_cgi(request, context, this);
		}
		catch (Cgi::CgiException())
		{
			this->status_code = INTERNAL_SERVER_ERROR;
			file.close();
			return ;
		}
		header["Content-Length"] = body_cgi.size();
		body.first.clear();
		body.second.clear();
	}
	else
		this->status_code = NOT_FOUND;
	file.close();
}

void	Response::handle_delete_method(Request &request, Context *context)
{
	(void) request;
	(void) context; //?
	std::ifstream file(this->path.c_str());

	bool is_dir = file.good() && !file.rdbuf()->in_avail();
	file.close();

	if (is_dir)
	{
		this->status_code = CONFLICT;
		return ;
	}
	int	ret = remove(this->path.c_str());
	if (ret == 0)
		this->status_code = OK;
	else if (errno == 2)
		this->status_code = NOT_FOUND;
	else
		this->status_code = FORBIDDEN;
	set_response();
}

void	Response::handle_request_by_method(Request &request, Context *context)
{
	switch (request.startline["method"].front())
	{
		case 'G' :
			handle_get_method(request, context);
			break;
		case 'P' :
			handle_post_method(request, context);
			break;
		case 'D' :
			handle_delete_method(request, context);
			break;
	}
}

void	Response::set_response()
{
	std::ostringstream os;
	time_t clock = time(0);
	char *clock_to_char = ctime(&clock);
	std::string status_line = get_status_line(this->status_code);
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
	if (default_error_page.find(status_code) != default_error_page.end())
	{
		body.first = default_error_page[status_code];
		body.second = "text/html";
	}
	if (body.first.size())
	{
		os << "Content-Length: " << body.first.size() << "\r\n";
		os << "Content-Type: " << body.second << "\r\n";
		os << body.first;
	}
	else if (body_cgi.size())
		os << body_cgi;
	else if (body.first.empty() && status_code >= 400)
	{
		std::string tmp = "<h1>" + status_line + "<h1>";
		os << "Content-Length: " << tmp.size() << "\r\n";
		os << "Content_Type: text/html\r\n\r\n" << tmp;
	}
	else
	{
		os << "Content-Length: 0\r\n";
		os << "Content-Type: text/html\r\n" << std::endl;
	}
	this->ready_to_send = os.str();
}

void	Response::set_default_error_page(Context *context)
{
	const Context *http = context->get_parent();
	std::map<std::string, std::vector<std::string> > directive = http->get_directive();
	
	for (std::map<std::string, std::vector<std::string> >::iterator it = directive.begin(); it != directive.end(); it++)
	{
		std::pair<std::string, std::vector<std::string> > tmp = *it;
		if (tmp.first.find("default_error_page") != std::string::npos)
		{
			const unsigned long pos = tmp.first.find_first_not_of("default_error_page");
			if (pos == std::string::npos)
				Response::parsingException();
			std::string new_tmp = tmp.first.substr(pos, tmp.first.size());
			this->default_error_page[std::atoi(new_tmp.c_str())] = tmp.second.front();
		}
	}
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
	}
}

void	Response::make_error_response(Response &response, std::pair<Request, Context *> &response_set, int status_code)
{
	(void) response;
	set_default_error_page(response_set.second);
	this->status_code = status_code;
	set_response();
}
