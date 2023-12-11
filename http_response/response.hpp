#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include "../http_request/request.hpp"

class Response 
{
	private:
		std::string method;
		std::string uri;
		std::string body;
		std::map<std::string, std::string> header;
		int	error_code;
	public:
		int	init_http_response(Request &request, int status);
		void	make_http_response(void);
};

#endif