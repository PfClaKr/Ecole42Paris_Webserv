#ifndef CGI_HPP
# define CGI_HPP

#include "response.hpp"
#include "../http_request/request.hpp"

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
			HTTP_ACCEPT_ENVODING,
			HTTP_ACCEPT_LANGUAGE,
			HTTP_FORWARDED,
			HTTP_HOST,
			HTTP_PROXY_AUTHORIZATION,
			HTTP_USER_AGENT,
			PATH_INFO,
			PATH_TRANSLATED,
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
};

#endif