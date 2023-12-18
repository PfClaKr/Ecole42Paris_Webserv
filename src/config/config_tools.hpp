#ifndef CONFIG_TOOLS_HPP
# define CONFIG_TOOLS_HPP

# include "context.hpp"

void	                print_config(Context &config);
std::vector<Context *>	get_context_by_name(Context &c, const std::string &name);
void	                print_contexts(const std::vector<Context *> &v);

#endif
