#ifndef CONTEXT_HPP
# define CONTEXT_HPP

# include <iostream>
# include <vector>
# include <map>
# include <string>
# include <exception>


class Context
{
	std::string name;
	std::vector<std::string> args;
	std::map<std::string, std::vector<std::string> > directive;
	std::vector<Context *> child;

public:
	void	set_args(const std::vector<std::string> args);
	void	add_directive(const std::string k, const std::vector<std::string> v);
	void	add_child(Context *child);

	const std::string					&get_name(void) const;
	const std::vector<std::string>				&get_args(void) const;
	const std::map<std::string, std::vector<std::string> >	&get_directive(void) const;
	const std::vector<std::string>				&get_directive_by_key(std::string k) const;
	const std::vector<Context *>				&get_child(void) const;

	Context(){
		this->name = "main";
	};
	Context(std::string name){
		this->name = name;
		// if need to set default value:
		if (this->name.compare("server") == 0)
		{
			this->directive["host"].push_back("0.0.0.0");
			this->directive["listen"].push_back("80");
		}
	};
        Context(Context &src)
        {
                std::cerr << "copy constructor is not allowed";
                throw FatalErrorException();
        }
        Context &operator=(Context &src)
        {
                std::cerr << "copy assignement operator is not allowed";
                throw FatalErrorException();
        }
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
