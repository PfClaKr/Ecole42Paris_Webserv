#include "config_parser.hpp"

static void sp(int n)
{
	for (int i = 0; i < n; i++)
		std::cout << " ";
}

static void print_context(Context &c, int indent = 0)
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
		std::cout << (*c.get_child()[i]).get_name();
		for (unsigned long j = 0; j < (*c.get_child()[i]).get_args().size(); j++)
			std::cout << " " << (*c.get_child()[i]).get_args()[j];
		std::cout << " {\n";
		print_context(*c.get_child()[i], indent + 4);
		sp(indent);
		std::cout << "}\n";
	}
}

void print_config(Context &config)
{
	print_context(config);
}

void get_servers(Context &c, std::vector<Context *> &servers)
{
	if (c.get_name().compare("server") == 0)
		servers.push_back(&c);
	for (unsigned long i = 0; i < c.get_child().size(); i++)
	{
		get_servers(*c.get_child()[i], servers);
	}
}

void print_servers(std::vector<Context *> &servers)
{
	for (unsigned long i = 0; i < servers.size(); i++)
	{
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		for (it = servers[i]->get_directive().begin(); it != servers[i]->get_directive().end(); it++)
		{
			std::cout << it->first;
			for (unsigned long j = 0; j < it->second.size(); j++)
				std::cout << " " << it->second[j];
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}