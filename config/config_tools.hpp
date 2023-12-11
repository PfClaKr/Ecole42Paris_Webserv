#ifndef CONFIG_TOOLS_HPP
# define CONFIG_TOOLS_HPP

# include "context.hpp"

void	print_config(Context &config);
void	get_servers(Context &c, std::vector<Context *> &servers);
void	print_servers(std::vector<Context *> &servers);

#endif
