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
