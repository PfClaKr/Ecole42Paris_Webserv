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
			REMOTE_ADDR,
			REMOTE_HOST,
			REMOTE_USER,
			REQUEST_METHOD,
			SCRIPT_NAME,
			SERVER_NAME,
			SERVER_PORT,
			SERVER_PROTOCOL,
			SERVER_SOFTWARE,
			LEN_OF_ENUM
		};
		char *env[LEN_OF_ENUM];
		std::string output;
		std::string path;
		std::string file;

		void set_cgi_meta_variable(Request &request, Context *context, std::string file, std::string query);
		void set_cgi_path(std::string file, Context *context);
		void run_cgi(Request &request);
		std::string set_output_in_response_body();
		Cgi(const Cgi &ref);
	public:
		Cgi();
		~Cgi();
		std::string init_cgi(Request &request, Context *context, Response *response);

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
