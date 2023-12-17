#include "response.hpp"

void	Response::check_host_in_header(Request &request)
{
	std::string host = request.header["Host"];
	if (host.empty())
	{
		this->status_code = BAD_REQUEST;
		throw (Response::responseException());
	}
}

void	Response::check_body_size(Request &request, Context *context)
{
	int	content_length = std::atoi(request.header["Contetn_length"].c_str());
	int max_body_size = std::atoi(context->get_directive_by_key("client_max_body_size")[0].c_str());
	int request_body_size = request.body.size();

	if (request_body_size > content_length)
	{
		this->status_code = PAYLOAD_TOO_LARGE;
		throw (Response::responseException());
	}
	if (request_body_size > max_body_size)
	{
		this->status_code = BAD_REQUEST;
		throw (Response::responseException());
	}
}

void	Response::check_method_allow(Request &request, Context *context)
{
	std::string request_method = request.startline["method"];
	std::vector<std::string> allowed_method = context->get_directive_by_key("allow_methods");

	for (int i = 0; i < allowed_method.size(); i++)
	{
		if (allowed_method[i] == request_method)
			return ;
	}
	this->status_code = METHOD_NOT_ALLOWED;
	throw (Response::responseException());
}

void	Response::check_request_uri(Request &request, Context *context)
{
	if (request.startline["uri"].front() != '/')
	{
		this->status_code = FORBIDDEN;
		throw (Response::responseException());
	}
}

void	Response::set_root_index_path(Request &request, Context *context)
{
	std::string root = context->get_directive_by_key("root")[0];
	std::string index = context->get_directive_by_key("index")[0];
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
	else if (request_path == "/")
		this->path = root + index;
	else
		this->path = root + request_path;
	this->query = "";
}

void	Response::check_integrity_request(std::pair<Request, Context *> &response_set)
{
	check_host_in_header(response_set.first);
	check_body_size(response_set.first, response_set.second);
	check_method_allow(response_set.first, response_set.second);
	check_request_uri(response_set.first, response_set.second);
}

void	Response::handle_get_method(Request &request, Context *context)
{
	std::fstream file;

	file.open(this->path.c_str(), std::ios::in);
}

void	Response::handle_post_method(Request &request, Context *context)
{

}

void	Response::handle_delete_method(Request &request, Context *context)
{
	std::ifstream file(this->path.c_str());

	bool is_dir = file.good() && !file.rdbuf()->in_avail();
	file.close();

	if (is_dir)
	{
		this->status_code = CONFLICT;
		throw Response::responseException();
	}
	int	ret = remove(this->path.c_str());
	if (ret == 0)
		this->status_code = OK;
	else if (errno == 2)
		this->status_code = NOT_FOUND;
	else
		this->status_code = FORBIDDEN;
	throw Response::responseException();
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

void	Response::make_http_response(Response &response, std::pair<Request, Context *> &response_set)
{
	try
	{
		check_integrity_request(response_set);
		set_root_index_path(response_set.first, response_set.second);
		handle_request_by_method(response_set.first, response_set.second);
	}
	catch (Response::responseException())
	{
	}
}