#include "response.hpp"

void	Response::make_http_response()
{

}

int	Response::init_http_response(Request &request, int status)
{
	return 1;	
}

void	http_response(Request &request, int status)
{
	Response response;

	response.init_http_response(request, status);
}