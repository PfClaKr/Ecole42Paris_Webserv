#include "config.hpp"


bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

std::vector<std::string> str_split_vec(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> v(begin, end);
	return v;
}

std::string context_name(std::string str)
{
	str.erase(0, str.find_first_not_of(" \t\v\r"));
	str = str.substr(0, str.find_first_of(" \t\v\r"));
	return str;
}

int count_char_in_str(std::string s, char c)
{
	int count = 0;
	for (unsigned long i = 0; i < s.length(); i++)
	{
		if (s[i] == c)
		{
			count++;
		}
	}
	return count;
}

int lexer_config_directives(std::string str)
{
	if (str.empty())
		return 0;
	if (count_char_in_str(str, '}') != 0 || count_char_in_str(str, '{') != 0 || count_char_in_str(str, ';') != 1)
		return -1;
	if (count_char_in_str(str, ' ') < 1 || str[str.length() - 1] != ';' || str[str.length() - 2] == ' ')
		return -1;
	return 0;
}

int lexer_context(std::string str)
{
	if (str.empty())
		return 0;
	if (str.find("{") != std::string::npos)
	{
		if (count_char_in_str(str, '{') != 1 || count_char_in_str(str, '}') != 0 || count_char_in_str(str, ';') != 0)
			return -1;
		if (count_char_in_str(str, ' ') < 1 || str[str.length() - 1] != '{' || str[str.length() - 2] != ' ')
			return -1;
	}
	else if (str.find("}") != std::string::npos)
	{
		if (count_char_in_str(str, '{') != 0 || count_char_in_str(str, '}') != 1 || count_char_in_str(str, ';') != 0)
			return -1;
		if (str.compare("}") != 0 || str.length() != 1)
			return -1;
	}
	return 0;
}

void preprocess(std::string &str)
{
	str = str.substr(0, str.find("#", 0));
	std::replace(str.begin(), str.end(), '\t', ' ');
	std::replace(str.begin(), str.end(), '\r', ' ');
	std::replace(str.begin(), str.end(), '\n', ' ');
	std::replace(str.begin(), str.end(), '\v', ' ');
	std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
	str.erase(new_end, str.end());
	str.erase(0, str.find_first_not_of(" \t\v\r"));
	str.erase(str.find_last_not_of(" \t\v\r") + 1);
}

int parse_context_directives(Context &config, std::string str)
{
	preprocess(str);
	if (lexer_config_directives(str) == -1)
		throw Context::SyntaxErrorException();
	if (str.empty())
		return 0;
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

int parse_context(Context &config, std::fstream &config_file)
{
	std::string str;
	while (!config_file.eof() && std::getline(config_file, str))
	{
		preprocess(str);
		if (lexer_context(str) == -1)
			throw Context::SyntaxErrorException();
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
			if (args.size() > 1)
			{
				args.erase(args.end() - 1);
				config.child.back()->args = args;
			}
			parse_context(*config.child.back(), config_file);
			continue ;
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
		throw Context::FatalErrorException();
	parse_context(config, config_file);
	config_file.close();
	return 0;
}

void sp(int n)
{
	for (int i = 0; i < n; i++)
		std::cout << " ";
}

void print_context(Context &c, int indent = 0)
{
	std::map<std::string, std::vector<std::string> >::iterator it;
	for (it = c.directive.begin(); it != c.directive.end(); it++)
	{
		sp(indent);
		std::cout << it->first;
		for (unsigned long i = 0; i < it->second.size(); i++)
		{
			std::cout << " " << it->second[i];
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
	print_context(config);
}

/*
*	TODO:
*	 - lexer
*	 - host:listen parsing/get (set default value?)
*	 - [server contexts] getter
*/

int main()
{
	Context config;
	std::cout << "parse config: \n\n";
	try
	{
		parse_config(config, "default_config");
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << "\n";
	}
	print_config(config);
	return 0;
}
