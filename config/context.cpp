#include "context.hpp"


void	Context::set_args(const std::vector<std::string> args)
{
        this->args = args;
}

void	Context::add_directive(const std::string k, const std::vector<std::string> v)
{
        this->directive[k] = v;
}

void	Context::add_child(Context *child)
{
        this->child.push_back(child);
}


const std::string &Context::get_name(void) const
{
        return this->name;
}

const std::vector<std::string> &Context::get_args(void) const
{
        return this->args;
}

const std::map<std::string, std::vector<std::string> > &Context::get_directive(void) const
{
        return this->directive;
}

const std::vector<std::string> &Context::get_directive_by_key(std::string k) const
{
        return this->directive.at(k); // exception throws if cannot find key
}

const std::vector<Context *> &Context::get_child(void) const
{
        return this->child;
}
