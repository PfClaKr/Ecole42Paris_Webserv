#include "cgi.hpp"

std::string	get_mime_type(std::string file)
{
	std::cout << DARK_BLUE << "file: " << file << " | " << file.substr(file.find_last_of(".") + 1, file.length() - file.find_last_of(".")) << RESET <<"\n";
	for (int i = 0; i < nbMime; i++)
	{
		if (mimeExt[i].compare(file.substr(file.find_last_of(".") + 1, file.length() - file.find_last_of("."))) == 0)
			return mimeType[i];
	}
	return ("text/plain");
}

void	Cgi::set_cgi_meta_variable(Request &request, Context *context, std::string file, std::string query)
{
	this->env = (char **)calloc(sizeof(char *), LEN_OF_ENUM + 1);
	this->env[AUTH_TYPE] = strdup(("AUTH_TYPE=" + request.header["auth-type"]).c_str());
	this->env[CONTENT_LENGTH] = strdup(("CONTENT_LENGTH=" + request.header["content-length"]).c_str());
	if (request.startline["method"] == "POST")
		this->env[CONTENT_TYPE] = strdup(("CONTENT_TYPE=" + request.header["content-type"]).c_str());
	else
		this->env[CONTENT_TYPE] = strdup(("CONTENT_TYPE=" + get_mime_type(file)).c_str());
	this->env[GATEWAY_INTERFACE] = strdup("GATEWAY_INTERFACE=CGI/1.1");
	this->env[HTTP_ACCEPT] = strdup(("HTTP_ACCEPT=" + request.header["accept"]).c_str());
	this->env[HTTP_ACCEPT_CHARSET] = strdup(("HTTP_ACCEPT_CHARSET=" + request.header["accept-charset"]).c_str());
	this->env[HTTP_ACCEPT_ENCODING] = strdup(("HTTP_ACCEPT_ENCODING=" + request.header["accept-encoding"]).c_str());
	this->env[HTTP_ACCEPT_LANGUAGE] = strdup(("HTTP_ACCEPT_LANGUAGE=" + request.header["accept-language"]).c_str());
	this->env[PATH_INFO] = strdup(("PATH_INFO=www/cgi-bin/php-cgi"));
	this->env[QUERY_STRING] = strdup(("QUERY_STRING=" + query).c_str());
	this->env[REDIRECT_STATUS] = strdup("REDIRECT_STATUS=200");
	this->env[REMOTE_ADDR] = strdup(("REMOTE_ADDR=" + context->get_directive_by_key("host")[0] +
		":" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[REMOTE_HOST] = strdup(("REMOTE_HOST=" + request.header["host"]).c_str());
	this->env[REMOTE_USER] = strdup("REMOTE_USER=user_name");
	this->env[REQUEST_METHOD] = strdup(("REQUEST_METHOD=" + request.startline["method"]).c_str());
	this->env[SCRIPT_FILENAME] = strdup(("SCRIPT_FILENAME=" + file).c_str());
	this->env[SCRIPT_NAME] = strdup(("SCRIPT_FILENAME=" + file).c_str());
	this->env[SERVER_NAME] = strdup("SERVER_NAME=ychunschae");
	this->env[SERVER_PORT] = strdup(("SERVER_PORT=" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[SERVER_PROTOCOL] = strdup(("SERVER_PROTOCOL=" + request.startline["http_version"]).c_str());
	this->env[SERVER_SOFTWARE] = strdup("SERVER_SOFTWARE=Nginx/2.0");
	this->env[UPLOAD_ERROR] = strdup(("STATUS_UPLOAD=" + this->status_upload).c_str());
	this->env[FILENAME] = strdup(("FILENAME=" + this->upload_file_name).c_str());
	this->env[LEN_OF_ENUM] = NULL;
}

void	Cgi::upload_file(std::string upload_file, std::string request_body)
{
	if (upload_file.empty())
		this->status_upload = "";
	std::ifstream tmp_file;
	std::string tmp_name;
	std::string patch = this->save_path + "/" + upload_file;
	tmp_file.open(patch.c_str());
	int	i = 1;
	while (tmp_file)
	{
		tmp_file.close();
		size_t delim = upload_file.find(".");
		tmp_name = upload_file;
		if (delim == std::string::npos)
		{
			std::stringstream ss;
			ss << i;
			std::string inc = "(" + ss.str() + ")";
			tmp_name = upload_file + inc;
			patch = this->save_path + "/" + tmp_name;
		}
		else
		{
			std::stringstream ss;
			ss << i;
			std::string inc = "(" + ss.str() + ")";
			tmp_name.insert(delim, inc);
			patch = this->save_path + "/" + tmp_name;
		}
		tmp_file.open(patch.c_str());
		i++;
	}
	if (tmp_name.empty())
		this->upload_file_name = upload_file;
	else
		this->upload_file_name = tmp_name;
	#ifdef DEBUG
		std::cout << CYAN << "Upload file name : " << this->upload_file_name << std::endl;
		std::cout << "status : " << this->status_upload << std::endl;
		std::cout << "patch : " << patch << RESET << std::endl;
	#endif

	std::ifstream check;
	std::ofstream file_ret(patch.c_str());
	check.open(patch.c_str());
	if (!check)
	{
		file_ret.close();
		return ;
	}
	file_ret << request_body;
	file_ret.close();
	this->status_upload = "0";
}

void	Cgi::run_cgi(Request &request)
{
	int fd[2];
	char ret[100000];

	FILE *file_in = tmpfile();
	int fd_in = fileno(file_in);
	int cp_stdin = dup(STDIN_FILENO);

	if (pipe(fd) == -1)
		throw Cgi::CgiException();
	if (write(fd_in, request.body.c_str(), request.body.size()) == -1)
		throw Cgi::CgiException();
	if (lseek(fd_in, 0, SEEK_SET) == -1)
		throw Cgi::CgiException();

	char **argv = (char **)calloc(sizeof(char *), 2 + 1);
	argv[0] = strdup(this->path.c_str());
	argv[1] = strdup(this->file.c_str());
	argv[2] = NULL;
	int pid = fork();
	if (pid == 0)
	{
		if (dup2(fd[1], STDOUT_FILENO) == -1)
			exit(-1);
		if (close(fd[0]) == -1)
			exit(-1);
		if (dup2(fd_in, STDIN_FILENO) == -1)
			exit(-1);
		execve(argv[0], argv, this->env);
	}
	else
	{
		waitpid(pid, NULL, -1);
		if (close(fd[1]) == -1)
			throw Cgi::CgiException();
		memset(ret, 0, sizeof(ret));
		this->output.clear();
		while (read(fd[0], ret, sizeof(ret)) > 0)
		{
			this->output += ret;
			memset(ret, 0, sizeof(ret));
		}
		if (close(fd[0]) == -1)
			throw Cgi::CgiException();
		dup2(STDIN_FILENO, cp_stdin);
		fclose(file_in);
		close(fd_in);
		close(cp_stdin);
	}
	for (int i = 0; i < 2; i++)
		free(argv[i]);
	free(argv);
}

void	Cgi::set_cgi_path(std::string file, Context *context)
{
	this->file = file;
	
	std::string root = context->get_parent()->get_directive_by_key("root").front();
	
	this->save_path = root + context->get_directive_by_key("save_path").front();
	std::vector<Context *> location = context->get_child();
	Context *cgi_bin;
	for (unsigned long i = 0; i < location.size(); i++)
	{
		if (location[i]->get_args().front() == "/cgi-bin")
		{
			cgi_bin = location[i];
			std::map<std::string, std::vector<std::string> > directive = cgi_bin->get_directive();
			this->path = directive["cgi_path"].front();
			#ifdef DEBUG
				std::cout << DARK_BLUE << "Cgi path : " << this->path << std::endl;
				std::cout << "Location block in config name was : " << location[i]->get_name() << std::endl;
				std::cout << "save_path : " << this->save_path << RESET << std::endl;
			#endif
			return ;
		}
	}
	throw Cgi::CgiException();
}

std::string	Cgi::set_output_in_response_body(Response *response)
{
	if (this->output.size() > 0)
	{
		std::stringstream ss;
		ss << this->output.size();
		std::string ret = ss.str();
		std::cout << RED << this->output << RESET << std::endl;
		response->set_header("Content-Length", ret);
		return ((this->output.substr(output.find("\r") + 4)));
	}
	else
	{
		response->set_header("Content_Length", "9");
		return ("No output");
	}
}

std::string	Cgi::init_cgi(Request &request, Context *context, Response *response)
{
	#ifdef DEBUG
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
	#endif
	set_cgi_path(response->get_path(), context);
	if (request.startline["method"] == "POST")
		upload_file(response->get_upload_file_name(), request.body);
	set_cgi_meta_variable(request, context, response->get_path(), response->get_query());
	run_cgi(request);
	for (int i = 0; i < LEN_OF_ENUM; i++)
		free(this->env[i]);
	free(env);
	return (set_output_in_response_body(response));
}

Cgi::Cgi()
{
}

Cgi::Cgi(const Cgi &ref)
{
	(void)ref;
}

Cgi::~Cgi()
{
}
