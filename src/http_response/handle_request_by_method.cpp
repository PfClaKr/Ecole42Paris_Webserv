#include "response.hpp"

static bool	check_is_php(std::string file)
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