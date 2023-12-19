#include "response.hpp"

void	Response::directory_autoindex()
{
	DIR *dir;
	struct dirent *dir_content;
	struct stat fileinfo;
	std::string body_tmp;

	if (!(dir = opendir(this->path.c_str())))
	{
		this->status_code = NOT_FOUND;
		return ;
	}
	else
	{
		#ifdef DEBUG
			std::cout << DARK_BLUE << "=================Auto index================" << std::endl;
			std::cout << "this->path : " << this->path << RESET << std::endl;
		#endif
		body_tmp += "<h1>Index of " + this->path + "<h1>\n";
		while ((dir_content = readdir(dir)) != NULL)
		{
			std::string tmp = std::string(dir_content->d_name);
			if (tmp != ".")
			{
				if (stat((this->path + "/" + tmp).c_str(), &fileinfo) == 0)
				{
					if (S_ISDIR(fileinfo.st_mode))
					{
						tmp += "/";
						body_tmp += "\t\t<a href=\"" + tmp + "\">" + tmp + "</a>";
					}
					else
						body_tmp += "\t\t<a href=\"" + tmp + "\">" + tmp + "</a>";
				}
			}
		}
	}
	closedir(dir);
	body.first = body_tmp;
	body.second = "text/html";
}