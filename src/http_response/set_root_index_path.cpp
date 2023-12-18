#include "response.hpp"

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
	if (set_location_in_request(request, context))
		;
	else
		this->path += request_path;
	this->query = "";
}