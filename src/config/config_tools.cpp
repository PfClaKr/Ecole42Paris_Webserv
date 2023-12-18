#include "config_parser.hpp"

static void sp(const int n)
{
	for (int i = 0; i < n; i++)
		std::cout << " ";
}

static void print_context(const Context &c, const int indent = 0)
{
	std::map<std::string, std::vector<std::string> >::const_iterator it;
	for (it = c.get_directive().begin(); it != c.get_directive().end(); it++)
	{
		sp(indent);
		std::cout << it->first;
		for (unsigned long i = 0; i < it->second.size(); i++)
		{
			std::cout << " " << it->second[i];
		}
		std::cout << "\n";
	}
	for (unsigned long i = 0; i < c.get_child().size(); i++)
	{
		sp(indent);
		if (c.get_parent() == NULL)
			std::cout << "main.";
		else
			std::cout << (c.get_child()[i]->get_parent()->get_name()) << ".";
		std::cout << (*c.get_child()[i]).get_name();
		for (unsigned long j = 0; j < (*c.get_child()[i]).get_args().size(); j++)
			std::cout << " " << (*c.get_child()[i]).get_args()[j];
		std::cout << " {\n";
		print_context(*c.get_child()[i], indent + 4);
		sp(indent);
		std::cout << "}\n";
	}
}

void print_config(const Context &config)
{
	print_context(config);
}

static void traverse_context(Context &c, std::vector<Context *> &v, const std::string &s)
{
	if (c.get_name().compare(s) == 0)
		v.push_back(&c);
	for (unsigned long i = 0; i < c.get_child().size(); i++)
	{
		traverse_context(*c.get_child()[i], v, s);
	}
}

std::vector<Context *> get_context_by_name(Context &c, const std::string &name)
{
	std::vector<Context *> v;
	traverse_context(c, v, name);
	return v;
}

void print_contexts(const std::vector<Context *> &v)
{
	for (unsigned long i = 0; i < v.size(); i++)
	{
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		for (it = v[i]->get_directive().begin(); it != v[i]->get_directive().end(); it++)
		{
			std::cout << it->first;
			for (unsigned long j = 0; j < it->second.size(); j++)
				std::cout << " " << it->second[j];
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

static int http_integrity_check(const Context &config)
{
	std::map<std::string, std::vector<std::string> > dir = config.get_directive();
	if (dir.count("root") != 1 || dir.count("autoindex") != 1)
		return -1;
	std::map<std::string, std::vector<std::string> >::const_iterator it;
	for (it = dir.begin(); it != dir.end(); it++)
	{
		if ((*it).first.find("autoindex") != std::string::npos)
		{
			if ((*it).second[0].compare("on") != 0 && (*it).second[0].compare("off") != 0)
				return -1;
		}
		if ((*it).first.find("default_error_page_") != std::string::npos)
			return 0;
	}
	return -1;
}

static int server_integrity_check(const Context &config)
{
	std::map<std::string, std::vector<std::string> > dir = config.get_directive();
	if (dir.count("listen") != 1
			|| dir.count("host") != 1
			|| dir.count("client_max_body_size") != 1
			|| dir.count("index") != 1
			|| dir.count("save_path") != 1)
		return -1;
	try
	{
		std::vector<std::string> am(config.get_directive_by_key("allow_methods"));
		std::sort(am.begin(), am.end());
		for (unsigned long i = 0; i < am.size(); i++)
		{
			if (am[i].compare("DELETE") == 0
					|| am[i].compare("GET") == 0
					|| am[i].compare("POST") == 0)
				continue ;
			else
				return -1;
		}
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}

static int location_integrity_check(const Context &config)
{
	std::map<std::string, std::vector<std::string> > dir = config.get_directive();
	if (dir.count("index") + dir.count("return") != 1 && dir.count("cgi_path") + dir.count("cgi_extention") == 0)
		return -1;
	if (dir.count("index") + dir.count("return") == 0 && dir.count("cgi_path") + dir.count("cgi_extention") != 2)
		return -1;
	if (dir.count("index") + dir.count("return") == 1 && dir.count("cgi_path") + dir.count("cgi_extention") != 0)
		return -1;
	if (dir.count("index") + dir.count("return") > 1 && dir.count("cgi_path") + dir.count("cgi_extention") != 0)
		return -1;
	return 0;
}

static int directive_integrity_check(const Context &config)
{
	std::map<std::string, std::vector<std::string> > dir = config.get_directive();
	std::map<std::string, std::vector<std::string> >::const_iterator it;
	for (it = dir.begin(); it != dir.end(); it++)
	{
		if ((*it).first.compare("allow_methods") != 0)
		{
			if ((*it).second.size() != 1)
				return -1;
		}
	}
	return 0;
}

int config_integrity_check(const Context &config)
{
	if (directive_integrity_check(config) == -1)
		return -1;
	if (config.get_name().compare("http") == 0)
	{
		if (http_integrity_check(config) == -1)
			return -1;
	}
	else if (config.get_name().compare("server") == 0)
	{
		if (server_integrity_check(config) == -1)
			return -1;
	}
	else if (config.get_name().compare("location") == 0)
	{
		if (location_integrity_check(config) == -1)
			return -1;
	}
	for (unsigned long i = 0; i < config.get_child().size(); i++)
	{
		if (config_integrity_check(*config.get_child()[i]) == -1)
			return -1;
	}
	return 0;
}
