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
	const Context *parent;
	std::vector<Context *> child;

        Context(Context &src)
        {
		(void) src;
                std::cerr << "copy constructor is not allowed";
        }
        Context &operator=(Context &src)
        {
		(void) src;
                std::cerr << "copy assignement operator is not allowed";
		return *this;
        }

public:
	void	set_args(const std::vector<std::string> args);
	void	add_directive(const std::string k, const std::vector<std::string> v);
	void	set_parent(Context *parent);
	void	add_child(Context *child);

	const std::string					&get_name(void) const;
	const std::vector<std::string>				&get_args(void) const;
	const std::map<std::string, std::vector<std::string> >	&get_directive(void) const;
	const std::vector<std::string>				&get_directive_by_key(std::string k) const;
	const Context						*get_parent(void) const;
	const std::vector<Context *>				&get_child(void) const;

	Context(){
		this->name = "main";
		this->parent = NULL;
	};
	Context(std::string name){
		this->name = name;
	};
	~Context(){
		for (unsigned long i = 0; i < child.size(); i++)
		{
			delete child[i];
		}
	};
	// class FatalErrorException : public std::exception
	// {
	// 	virtual const char* what() const throw()
	// 	{
	// 		return "Fatal Error";
	// 	}
	// };
};

#endif
