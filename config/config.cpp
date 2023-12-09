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

void rm_after_delim(std::string &str, std::string delim)
{
	str = str.substr(0, str.find(delim, 0));
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
	rm_after_delim(str, "#");
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
		rm_after_delim(str, "#");
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
			std::vector<std::string> args(std::next(begin), end);
			if (args.size() > 1)
				config.child.back()->args = args;
			parse_context(*config.child.back(), config_file, str.erase(str.find("{")));
			continue ; // recursive exit
		}
		parse_context_directives(config, str);
	}
	return 0;
}

int parse_config(std::vector<Context *> &config, std::string filename)
{
	std::fstream config_file;
	config_file.open(filename, std::fstream::in);
	if (!config_file.is_open())
		return -1;
	config.push_back(new Context());
	for (std::string str; std::getline(config_file, str); )
	{
		rm_after_delim(str, "#");
		if (!str.empty() && str.find("{") != std::string::npos)
		{
			config.back()->child.push_back(new Context(context_name(str)));
			parse_context(*config.back()->child.back(), config_file, str.erase(str.find("{")));
		} else {
			if (!str.empty())
				parse_context_directives(*config[0], str);
		}
	}
	config_file.close();
	return 0;
}

void print_config(std::vector<Context *> config)
{
	for (int i = 0; i < config.size(); i++)
	{
		std::cout << "name[0] :\t." << config[i]->name << ".\n";
		for (int j = 0; j < config[i]->child.size(); j++)
		{
			std::cout << "name[1] :\t\t." << config[i]->child[j]->name << ".\n";
			for (int k = 0; k < config[i]->child[j]->child.size(); k++)
			{
				std::cout << "name[2] :\t\t\t." << config[i]->child[j]->child[k]->name << ".\n";
			}
		}
	}


	Context *main, *one, *two, *three, *four;
	
	main = config[0];
	one = config[0]->child[0];
	two = config[0]->child[1];
	three = two->child[0];
	four = three->child[0];

	std::cout << "name: " << main->name << "\n";
	std::cout << main->directive["pid"][0] << "\n";
	std::cout << "name: " << one->name << "\n";
	std::cout << one->directive["worker_connections"][0] << "\n";
	std::cout << "name: " << two->name << "\n";
	std::cout << two->directive["index"][0] << " " << two->directive["index"][1] << " " << two->directive["index"][2] << "\n";
	std::cout << "name: " << three->name << "\n";
	std::cout << "name: " << four->name << "\n";
	std::cout << "args: " << four->args[0] << " " << four->args[1] << "\n";
	std::cout << four->directive["fastcgi_pass"][0] << "\n";
}

int main()
{
	std::vector<Context *> config;
	std::cout << "parse config: \n\n";
	if (parse_config(config, "default_config") == -1)
		return 0;
	print_config(config);
	for (int i = 0; i < config.size(); i++)
		delete config[i];
	return 0;
}
