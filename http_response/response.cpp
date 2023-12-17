#include "response.hpp"

bool	Response::check_integrity_request(std::pair<Request, Context *> &config)
{
	check_host_in_header();
	check_body_size();

}

void	Response::make_http_response(Response &response, std::pair<Request, Context *> &config)
{
	if (!check_integrity_request(config))
		this->set_error_page();
}