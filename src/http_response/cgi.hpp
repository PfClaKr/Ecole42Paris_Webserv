#ifndef CGI_HPP
# define CGI_HPP

#include <wait.h>
#include "response.hpp"
#include "mime.hpp"
#include "../http_request/request.hpp"
#include "../config/colors.hpp"

class Response;

class Cgi
{
	private:
		// https://datatracker.ietf.org/doc/html/rfc3875#page-11
		// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
		enum cgi_meta_variable
		{
			AUTH_TYPE,
			CONTENT_LENGTH,
			CONTENT_TYPE,
			GATEWAY_INTERFACE,
			HTTP_ACCEPT,
			HTTP_ACCEPT_CHARSET,
			HTTP_ACCEPT_ENCODING,
			HTTP_ACCEPT_LANGUAGE,
			PATH_INFO,
			QUERY_STRING,
			REDIRECT_STATUS,
			REMOTE_ADDR,
			REMOTE_HOST,
			REMOTE_USER,
			REQUEST_METHOD,
			SCRIPT_FILENAME,
			SCRIPT_NAME,
			SERVER_NAME,
			SERVER_PORT,
			SERVER_PROTOCOL,
			SERVER_SOFTWARE,
			UPLOAD_ERROR,
			FILENAME,
			LEN_OF_ENUM
		};
		char **env;
		std::string output;
		std::string save_path;
		std::string upload_file_name;
		std::string path;
		std::string file;
		std::string	status_upload;

		void set_cgi_meta_variable(Request &request, Context *context, std::string file, std::string query);
		void set_cgi_path(std::string file, Context *context);
		void run_cgi(Request &request);
		std::string set_output_in_response_body(Response *response);
		Cgi(const Cgi &ref);
	public:
		Cgi();
		~Cgi();
		std::string init_cgi(Request &request, Context *context, Response *response);
		void	upload_file(std::string upload_file, std::string request_body);

		class CgiException : public std::exception
		{
		public:
			virtual const char *what() const throw()
			{
				return "Cgi Exception";
			};
		};
};

std::string get_mime_type(std::string file);

#endif
