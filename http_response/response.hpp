#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include "../http_request/request.hpp"
# include "../config/context.hpp"

class Response 
{
	public:
		void make_http_response(Response &response, std::pair<Request, Context *> &config);
};

#endif