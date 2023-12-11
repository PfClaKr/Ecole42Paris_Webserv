#include "config_parser.hpp"

static bool BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}

static std::vector<std::string> str_split_vec(std::string s)
{
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> v(begin, end);
	return v;
}

static std::string context_name(std::string str)
{
	str.erase(0, str.find_first_not_of(" \t\v\r"));
	str = str.substr(0, str.find_first_of(" \t\v\r"));
	return str;
}

static int count_char_in_str(std::string s, char c)
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

static int lexer_config_directives(std::string str)
{
	if (str.empty())
		return 0;
	if (count_char_in_str(str, '}') != 0 || count_char_in_str(str, '{') != 0 || count_char_in_str(str, ';') != 1)
		return -1;
	if (count_char_in_str(str, ' ') < 1 || str[str.length() - 1] != ';' || str[str.length() - 2] == ' ')
		return -1;
	return 0;
}

static int lexer_context(std::string str)
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

static void preprocess(std::string &str)
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

static void parse_context_directives(Context &config, std::string str)
{
	preprocess(str);
	if (lexer_config_directives(str) == -1)
		throw Context::SyntaxErrorException();
	if (str.empty())
		return ;
	char s[str.length() + 1];
	std::strcpy(s, str.c_str());
	char *p = strtok(s, ";");
	while (p != NULL)
	{
		std::vector<std::string> v = str_split_vec(p);
		std::string k = v[0];
		v.erase(v.begin());
		config.add_directive(k, v);
		p = strtok(NULL, ";");
	}
}

static void parse_context(Context &config, std::fstream &config_file, int &context_level)
{
	std::string str;
	while (!config_file.eof() && std::getline(config_file, str))
	{
		preprocess(str);
		if (lexer_context(str) == -1)
			throw Context::SyntaxErrorException();
		if (str.find("}") != std::string::npos)
		{
			context_level--;
			break ;
		}
		if (context_level < 0)
			throw Context::SyntaxErrorException();
		if (str.empty())
			continue ;
		if (str.find("{") != std::string::npos)
		{
			context_level++;
			config.add_child(new Context(context_name(str)));
			std::stringstream ss(str);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> args(++begin, end);
			if (args.size() > 1)
			{
				args.erase(args.end() - 1);
				config.get_child().back()->set_args(args);
			}
			parse_context(*config.get_child().back(), config_file, context_level);
			continue ;
		}
		parse_context_directives(config, str);
	}
}

void parse_config(Context &config, std::string filename)
{
	std::fstream config_file;
	config_file.open(filename.c_str(), std::fstream::in);
	if (!config_file.is_open())
		throw Context::FatalErrorException();
	int context_level = 0;
	parse_context(config, config_file, context_level);
	if (context_level != 0)
		throw Context::SyntaxErrorException();
	config_file.close();
}

// #include "config_tools.hpp"

// int main()
// {
// 	Context config;
// 	std::cout << "parse config: \n\n";
// 	try
// 	{
// 		parse_config(config, "default_config");
// 	}
// 	catch (std::exception &e)
// 	{
// 		std::cerr << e.what() << "\n";
// 		return 0;
// 	}
// 	print_config(config);
// 	try
// 	{
// 		config.get_directive_by_key("test");
// 	}
// 	catch (std::exception &e)
// 	{
// 		std::cerr << "exception raised on " << e.what() << "\n";
// 	}
// 	std::vector<Context *> servers;
// 	get_servers(config, servers);
// 	return 0;
// }
