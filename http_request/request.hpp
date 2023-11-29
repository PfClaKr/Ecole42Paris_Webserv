#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <cstring>
# include <map>
# include <bits/stdc++.h>

class Request
{
	public:
		std::map<std::string, std::string> startline;
		std::map<std::string, std::string> header;
	public:
		Request(){};

};

#endif
