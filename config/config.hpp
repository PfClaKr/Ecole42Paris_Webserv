#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <cstring>
# include <map>
# include <vector>
# include <bits/stdc++.h>
# include <fstream>
# include <iterator>
# include <algorithm>

class Context
{
public:
	std::string name;
	std::vector<std::string> args;
	std::map<std::string, std::vector<std::string> > directive;
	std::vector<Context *> child;

	Context(){
		this->name = "main";
	};
	Context(std::string name){
		this->name = name;
		// if need to set default value:
		//
		// if (this->name.compare("server") == 0)
		// {
		// 	this->directive["host"].push_back("0.0.0.0");
		// 	this->directive["listen"].push_back("80");
		// }
	};
	~Context(){
		for (unsigned long i = 0; i < child.size(); i++)
		{
			delete child[i];
		}
	};
	class FatalErrorException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "Fatal Error";
		}
	};
	class SyntaxErrorException : public std::exception
	{
		virtual const char* what() const throw()
		{
			return "Syntax Error";
		}
	};
};


#endif
