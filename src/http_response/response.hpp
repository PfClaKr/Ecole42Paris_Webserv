#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <dirent.h>
# include <sys/stat.h>
# include "status_code.hpp"
# include "../config/colors.hpp"
# include "../http_request/request.hpp"
# include "../config/context.hpp"
# include "cgi.hpp"

class Response 
{
	private:
		int	status_code;
		std::string path;
		std::string query;
		std::string location;
		std::string upload_file_name;
		std::map<std::string, std::string> header;
		std::map<int, std::string> default_error_page;
		std::pair<std::string, std::string> body;
		std::string body_cgi;
		std::string ready_to_send;

		int check_integrity_request(std::pair<Request, Context *> &response_set);
		int check_host_in_header(Request &request);
		int check_body_size(Request &request, Context *context);
		int check_method_allow(Request &request, Context *context);
		int check_request_uri(Request &request, Context *context);
		void set_root_index_path(Request &request, Context *context);
		bool set_location_in_request(Request &request, Context *context, std::string root);
		void check_index_or_redirection(Request &request, Context *context, std::string root);
		void handle_request_by_method(Request &request, Context *context);
		void handle_get_method(Request &request, Context *context);
		void handle_post_method(Request &request, Context *context);
		void handle_delete_method(Request &request);
		void directory_autoindex();
		void set_response(Request &request);
		void set_default_error_page(Context *context);
		void set_response_error_page();
		bool process_request_body_if_multipart(Request &request);
		bool process_unchunk(Request &request);
		Response(const Response &ref);
	public:
		Response();
		~Response();
		// void unchunk(Request &request);
		void make_http_response(std::pair<Request, Context *> &response_set);
		void make_error_response(std::pair<Request, Context *> &response_set, int status_code);
		std::string get_path();
		std::string get_query();
		std::string get_status_line(int status);
		std::string get_ready_to_send();
		std::string get_upload_file_name();
		void set_header(std::string header, std::string value);

		class parsingException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return "Parsing Error Exception";
				};
		};
};

#endif
