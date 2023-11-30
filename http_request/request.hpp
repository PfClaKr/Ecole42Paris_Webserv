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
};

#endif
