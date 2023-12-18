#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <cstring>
# include <map>
# include <vector>
# include <bits/stdc++.h>

class Request
{
	public:
		std::map<std::string, std::string> startline;
		std::map<std::string, std::string> header;
		std::string body;
	public:
		Request(){
			this->startline["method"] = "";
			this->startline["uri"] = "";
			this->startline["http_version"] = "";
			this->body = "";
		};
		void	clear_value();
		class parsingException : public std::exception
		{
			public:
				const char *what() const throw()
				{
					return "Parsing Error Exception";
				};
		};
};

int parse_http_request(Request &request, std::string req);
void print_http_request(Request &request);

#endif
