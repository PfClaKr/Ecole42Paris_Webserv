#include "config.hpp"


bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

std::string context_name(std::string str)
{
	str.erase(0, str.find_first_not_of(" \t\v\r"));
	str = str.substr(0, str.find_first_of(" \t\v\r"));
	return str;
}

void temp_trim(std::string &s)
{
	// std::cout << "test: " << s.find_first_not_of(" \t\v\r\n") << "\n";
	// if (s.find_first_not_of(" \t\v\r\n") == std::string::npos)
	// {
	// 	s.clear();
	// 	return ;
	// }
	s.erase(0, s.find_first_not_of(" \t\v\r")); //str trim
	s.erase(s.find_last_not_of(" \t\v\r") + 1);
}

void preprocess(std::string &str)
{
	str = str.substr(0, str.find("#", 0));
	std::replace(str.begin(), str.end(), '\t', ' ');
	std::replace(str.begin(), str.end(), '\r', ' ');
	std::replace(str.begin(), str.end(), '\n', ' ');
	std::replace(str.begin(), str.end(), '\v', ' ');
	temp_trim(str);
}

std::vector<std::string> str_split_vec(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> v(begin, end);
	return v;
}

bool is_comment(char *s)
{
	std::string str = s;
	if (str[str.find_first_not_of(" \t\v\r")] == '#')
		return 1;
	return 0;
}

int parse_context_directives(Context &config, std::string str)
{
	preprocess(str);
	if (str.empty())
		return 0;
	std::replace(str.begin(), str.end(), '\t', ' ');
	std::replace(str.begin(), str.end(), '\v', ' ');
	std::replace(str.begin(), str.end(), '\r', ' ');
	std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
	str.erase(new_end, str.end());  
	str.erase(0, str.find_first_not_of(" ")); //str trim
	str.erase(str.find_last_not_of(" ") + 1);
	char s[str.length() + 1];
	std::strcpy(s, str.c_str());
	char *p = strtok(s, ";");
	while (p != NULL)
	{
		std::vector<std::string> v = str_split_vec(p);
		std::string k = v[0];
		v.erase(v.begin());
		config.directive[k] = v;
		p = strtok(NULL, ";");
	}
	return 0;
}

int parse_context(Context &config, std::fstream &config_file, std::string str)
{
	// std::cout << "context name: " << str << "\n";
	while (!config_file.eof() && std::getline(config_file, str))
	{
		preprocess(str);
		if (str.find("}") != std::string::npos)
			break ;
		if (str.empty())
			continue ;
		if (str.find("{") != std::string::npos)
		{
			config.child.push_back(new Context(context_name(str)));
			std::stringstream ss(str);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> args(++begin, end);
			/*
			* Need to find another solution for parsing context and context name
			* 	XXXXX { - good
			* 	XXXXX
			* 	{       - bad
			* 	XXXXX{  - also bad
			*/
			if (args.size() > 1)
			{
				args.erase(args.end() - 1);
				config.child.back()->args = args;
			}
			parse_context(*config.child.back(), config_file, str.erase(str.find("{")));
			continue ; // recursive exit
		}
		parse_context_directives(config, str);
	}
	return 0;
}

int parse_config(Context &config, std::string filename)
{
	std::fstream config_file;
	config_file.open(filename.c_str(), std::fstream::in);
	if (!config_file.is_open())
		return -1;
	for (std::string str; std::getline(config_file, str); )
	{
		preprocess(str);
		if (!str.empty() && str.find("{") != std::string::npos)
		{
			config.child.push_back(new Context(context_name(str)));
			parse_context(*config.child.back(), config_file, str.erase(str.find("{")));
		} else {
			if (!str.empty())
				parse_context_directives(config, str);
		}
	}
	config_file.close();
	return 0;
}
#include <iomanip>
void sp(int n)
{
	for (int i = 0; i < n; i++)
		std::cout << " ";
}
void print_context(Context &c, int indent)
{
	std::map<std::string, std::vector<std::string> >::iterator it;
	for (it = c.directive.begin(); it != c.directive.end(); it++)
	{
		sp(indent);
		std::cout << it->first << " ";
		for (unsigned long i = 0; i < it->second.size(); i++)
		{
			std::cout << it->second[i] << " ";
		}
		std::cout << "\n";
	}
	for (unsigned long i = 0; i < c.child.size(); i++)
	{
		sp(indent);
		std::cout << (*c.child[i]).name;
		for (unsigned long j = 0; j < (*c.child[i]).args.size(); j++)
			std::cout << " " << (*c.child[i]).args[j];
		std::cout << " {\n";
		print_context(*c.child[i], indent + 4);
		sp(indent);
		std::cout << "}\n";
	}
	return ;
}

void print_config(Context &config)
{
	print_context(config, 0);
}

int main()
{
	Context config;
	std::cout << "parse config: \n\n";
	if (parse_config(config, "default_config") == -1)
		return 0;
	print_config(config);
	return 0;
}
