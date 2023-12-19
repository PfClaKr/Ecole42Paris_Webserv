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
	this->env[PATH_INFO] = strdup(("PATH_INFO=" + file).c_str());
	this->env[QUERY_STRING] = strdup(("QUERY_STRING=" + query).c_str());
	this->env[REDIRECT_STATUS] = strdup("REDIRECT_STATUS=200");
	this->env[REMOTE_ADDR] = strdup(("REMOTE_ADDR=" + context->get_directive_by_key("host")[0] +
		":" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[REMOTE_HOST] = strdup(("REMOTE_HOST=" + request.header["host"]).c_str());
	this->env[REMOTE_USER] = strdup("REMOTE_USER=user_name");
	this->env[REQUEST_METHOD] = strdup(("REQUEST_METHOD=" + request.startline["method"]).c_str());
	this->env[SCRIPT_NAME] = strdup(("SCRIPT_NAME=" + file).c_str());
	this->env[SERVER_NAME] = strdup("SERVER_NAME=ychunschae");
	this->env[SERVER_PORT] = strdup(("SERVER_PORT=" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[SERVER_PROTOCOL] = strdup(("SERVER_PROTOCOL=" + request.startline["http_version"]).c_str());
	this->env[SERVER_SOFTWARE] = strdup("SERVER_SOFTWARE=Nginx/2.0");
	for (int i = 0; i < 20; i++)
		std::cout << this->env[i] << std::endl;
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

	char **argv = new char*[3];
	argv[0] = strdup(this->path.c_str());
	argv[1] = strdup(this->file.c_str());
	argv[2] = NULL;
	std::cout << RED << "argv[0]: " << argv[0] << std::endl;
	std::cout << "argv[1]: " << argv[1] << RESET << std::endl;
	int pid = fork();
	if (pid)
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
		for (int i = 0; i < 3; i++)
			delete[] argv[i];
		delete[] argv;
	}
}

void	Cgi::set_cgi_path(std::string file, Context *context)
{
	this->file = file;
	
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
				std::cout << "Location block in config name was : " << location[i]->get_name() << RESET << std::endl;
			#endif
			return ;
		}
	}
	throw Cgi::CgiException();
}

std::string	Cgi::set_output_in_response_body()
{
	if (this->output.size() > 0)
		return ((this->output.substr(output.find("\r") + 4)));
	else
		return ("No output");
	
}

std::string	Cgi::init_cgi(Request &request, Context *context, Response *response)
{
	#ifdef DEBUG
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
		std::cout << DARK_BLUE << "==============CGI===============" << RESET << std::endl;
	#endif
	set_cgi_meta_variable(request, context, response->get_path(), response->get_query());
	std::cerr << RED << "WHAT?\n" << RESET;
	set_cgi_path(response->get_path(), context);
	run_cgi(request);
	return (set_output_in_response_body());
}

Cgi::Cgi()
{
	this->env = new char*[LEN_OF_ENUM];
}

Cgi::Cgi(const Cgi &ref)
{
	(void)ref;
}

Cgi::~Cgi()
{
	for (int i = 0; i < LEN_OF_ENUM; i++)
		delete[] env[i];
	delete[] env;
}