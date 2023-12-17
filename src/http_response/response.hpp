#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include "../http_request/request.hpp"
# include "../http_request/status_code.hpp"
# include "../config/context.hpp"

class Response 
{
	private:
		int	status_code;
		std::string path;
		std::string query;
		std::string body;
		std::string location;
		bool is_autoindex;

		void check_integrity_request(std::pair<Request, Context *> &response_set);
		void check_host_in_header(Request &request);
		void check_body_size(Request &request, Context *context);
		void check_method_allow(Request &request, Context *context);
		void check_request_uri(Request &request, Context *context);
		void set_root_index_path(Request &request, Context *context);
		void handle_request_by_method(Request &request, Context *context);
		void handle_get_method(Request &request, Context *context);
		void handle_post_method(Request &request, Context *context);
		void handle_delete_method(Request &request, Context *context);
	public:
		void make_http_response(Response &response, std::pair<Request, Context *> &response_set);
		class responseException : public std::exception
		{
			virtual const char* what() const throw()
			{
				return "Reponse Error";
			}
		};
};

#endif