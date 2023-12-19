#include "config_parser.hpp"

static bool BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}

static std::string context_name(std::string str)
{
	str.erase(0, str.find_first_not_of(" \t\v\r"));
	str = str.substr(0, str.find_first_of(" \t\v\r"));
	return str;
}

static int count_char_in_str(const std::string &s, const char c)
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

static int lexer_config_directives(const std::string &str)
{
	if (str.empty())
		return 0;
	if (count_char_in_str(str, '}') != 0 || count_char_in_str(str, '{') != 0 || count_char_in_str(str, ';') != 1)
		return -1;
	if (count_char_in_str(str, ' ') < 1 || str[str.length() - 1] != ';' || str[str.length() - 2] == ' ')
		return -1;
	return 0;
}

static int lexer_context(const std::string &str)
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

static int parse_context_directives(Context &config, std::string &str)
{
	preprocess(str);
	if (lexer_config_directives(str) == -1)
		return -1;
	if (str.empty())
		return 0;
	str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
	std::stringstream ss(str);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> v(begin, end);
	std::string k = v[0];
	v.erase(v.begin());
	config.add_directive(k, v);
	return 0;
}

static int parse_context(Context &config, std::fstream &config_file, int &context_level)
{
	std::string str;
	while (!config_file.eof() && std::getline(config_file, str))
	{
		preprocess(str);
		if (lexer_context(str) == -1)
			return -1;
		if (str.find("}") != std::string::npos)
		{
			context_level--;
			break ;
		}
		if (context_level < 0)
			return -1;
		if (str.empty())
			continue ;
		if (str.find("{") != std::string::npos)
		{
			context_level++;
			config.add_child(new Context(context_name(str)));
			config.get_child().back()->set_parent(&config);
			std::stringstream ss(str);
			std::istream_iterator<std::string> begin(ss);
			std::istream_iterator<std::string> end;
			std::vector<std::string> args(++begin, end);
			if (args.size() > 1)
			{
				args.erase(args.end() - 1);
				config.get_child().back()->set_args(args);
			}
			if (parse_context(*config.get_child().back(), config_file, context_level) == -1)
				return -1;
			continue ;
		}
		if (parse_context_directives(config, str) == -1)
			return -1;
	}
	return 0;
}

int parse_config(Context &config, const std::string &filename)
{
	std::fstream config_file;
	config_file.open(filename.c_str(), std::fstream::in);
	if (!config_file.is_open())
		return -1;
	int context_level = 0;
	parse_context(config, config_file, context_level);
	config_file.close();
	if (context_level != 0)
		return -1;
	return 0;
}

// #include "config_tools.hpp"

// int main()
// {
// 	Context config;
// 	std::cout << "parse config: \n\n";
// 	if (parse_config(config, "default.conf") == -1)
// 	{
// 		std::cout << "\nerror\n";
// 		return 0;
// 	}
// 	print_config(config);
// 	// try
// 	// {
// 	// 	config.get_directive_by_key("test");
// 	// }
// 	// catch (std::exception &e)
// 	// {
// 	// 	std::cerr << "exception raised on " << e.what() << "\n";
// 	// }

// 	// std::vector<Context *> servers;
// 	// std::vector<Context *> http;
// 	// servers = get_context_by_name(config, "server");
// 	// http = get_context_by_name(config, "http");
// 	// print_contexts(servers);
// 	// print_contexts(http);
// 	return 0;
// }
