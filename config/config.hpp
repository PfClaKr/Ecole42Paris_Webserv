#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <cstring>
# include <map>
# include <vector>
# include <bits/stdc++.h>
# include <fstream>

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
	};
	~Context(){
		for (int i = 0; i < child.size(); i++)
		{
			delete child[i];
		}
	};

};


#endif
