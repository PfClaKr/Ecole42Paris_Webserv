#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <iostream>
# include <cstring>
# include <map>
# include <vector>
# include <bits/stdc++.h>
# include <fstream>
# include <iterator>
# include <algorithm>

# include "context.hpp"

int	parse_config(Context &config, const std::string &filename);

#endif
