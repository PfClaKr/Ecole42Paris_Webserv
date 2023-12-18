#include "cgi.hpp"

std::string	get_mime_type(std::string file)
{
	std::string extension;

	for (std::string::reverse_iterator it = file.rbegin(); it != file.rend(); it++)
	{
		if (*it == '.')
			break;
		extension += *it;
	}
	for (int i = 0; i < nbMime; i++)
	{
		if (mimeExt[i] == extension)
			return mimeType[i];
	}
	return ("text/plain");
}

void	Cgi::set_cgi_meta_variable(Request &request, Context *context, std::string file, std::string query)
{
	this->env[AUTH_TYPE] = const_cast<char *>(("AUTH_TYPE=" + request.header["AuthType"]).c_str());
	this->env[CONTENT_LENGTH] = const_cast<char *>(("CONTENT_LENGTH=" + request.header["Content-Length"]).c_str());
	if (request.startline["method"] == "POST")
		this->env[CONTENT_TYPE] = const_cast<char *>(("CONTENT_TYPE=" + request.header["Content-Type"]).c_str());
	else
		this->env[CONTENT_TYPE] = const_cast<char *>(("CONTENT_TYPE=" + get_mime_type(file)).c_str());
	this->env[GATEWAY_INTERFACE] = const_cast<char *>("GATEWAY_INTERFACE=CGI/1.1");
	this->env[HTTP_ACCEPT] = const_cast<char *>(("HTTP_ACCEPT=" + request.header["Accept"]).c_str());
	this->env[HTTP_ACCEPT_CHARSET] = const_cast<char *>(("HTTP_ACCEPT_CHARSET=" + request.header["Accept-Charset"]).c_str());
	this->env[HTTP_ACCEPT_ENCODING] = const_cast<char *>(("HTTP_ACCEPT_ENCODING=" + request.header["Accept-Encoding"]).c_str());
	this->env[HTTP_ACCEPT_LANGUAGE] = const_cast<char *>(("HTTP_ACCEPT_LANGUAGE=" + request.header["Accept-Language"]).c_str());
	this->env[PATH_INFO] = const_cast<char *>(("PATH_INFO=" + file).c_str());
	this->env[QUERY_STRING] = const_cast<char *>(("QUERY_STRING=" + query).c_str());
	this->env[REMOTE_ADDR] = const_cast<char *>(("REMOTE_ADDR=" + context->get_directive_by_key("host")[0] +
		":" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[REMOTE_HOST] = const_cast<char *>(("REMOTE_HOST=" + request.header["Host"]).c_str());
	this->env[REMOTE_USER] = const_cast<char *>("REMOTE_USER=user_name");
	this->env[REQUEST_METHOD] = const_cast<char *>(("REQUEST_METHOD=" + request.startline["method"]).c_str());
	this->env[SCRIPT_NAME] = const_cast<char *>(("SCRIPT_NAME=" + file).c_str());
	this->env[SERVER_NAME] = const_cast<char *>("SERVER_NAME=ychunschae");
	this->env[SERVER_PORT] = const_cast<char *>(("SERVER_PORT=" + context->get_directive_by_key("listen")[0]).c_str());
	this->env[SERVER_PROTOCOL] = const_cast<char *>(("SERVER_PROTOCOL" + request.startline["http_version"]).c_str());
	this->env[SERVER_SOFTWARE] = const_cast<char *>("SERVER_SOFTWARE=Nginx/2.0");
}

void	Cgi::run_cgi(Request &request, Context *context, std::string file)
{
	int fd[2];
	char ret[100000];
	(void) context;
	(void) file;

	FILE *file_in = tmpfile();
	int fd_in = fileno(file_in);
	int cp_stdin = dup(STDIN_FILENO);

	if (!pipe(fd))
		throw Cgi::CgiException();
	if (!write(fd_in, request.body.c_str(), request.body.size()))
		throw Cgi::CgiException();
	if (!lseek(fd_in, 0, SEEK_SET))
		throw Cgi::CgiException();
	

	int pid = fork();
	if (pid)
	{
		char *argv[3];
		argv[0] = (char *)this->path.c_str();
		argv[1] = (char *)this->file.c_str();
		argv[2] = NULL;
		if (!dup2(fd[1], STDOUT_FILENO))
			exit(-1);
		if (!close(fd[0]))
			exit(-1);
		if (!dup2(fd_in, STDIN_FILENO))
			exit(-1);
		if (!execve(argv[0], argv, env))
		{
			fclose(file_in);
			close(fd_in);
			close(cp_stdin);
			kill(getpid(), SIGTERM);
		}
	}
	else
	{
		waitpid(pid, NULL, -1);
		if (!close(fd[1]))
			throw Cgi::CgiException();
		memset(ret, 0, sizeof(ret));
		this->output.clear();
		while (read(fd[0], ret, sizeof(ret)) > 0)
		{
			this->output += ret;
			memset(ret, 0, sizeof(ret));
		}
		if (!close(fd[0]))
			throw Cgi::CgiException();
		dup2(STDIN_FILENO, cp_stdin);
		fclose(file_in);
		close(fd_in);
		close(cp_stdin);
	}
}

void	Cgi::set_cgi_path(std::string file, Context *context)
{
	this->file = file;
	
	std::vector<Context *> location = context->get_child();
	Context *cgi_bin;
	for (unsigned long i = 0; i < location.size(); i++)
	{
		if (location[i]->get_name() == "/cgi_bin")
		{
			cgi_bin = location[i];
			std::map<std::string, std::vector<std::string> > directive = cgi_bin->get_directive();
			this->path = directive["cgi_path"].front();
			return ;
		}
	}
	throw Cgi::CgiException();
}

std::string	Cgi::set_output_in_response_body(Response *response)
{
	(void) response;
	if (this->output.size() > 0)
		return ((this->output.substr(output.find("\r") + 4)));
	else
		return ("No output");
	
}

std::string	Cgi::init_cgi(Request &request, Context *context, Response *response)
{
	set_cgi_meta_variable(request, context, response->get_path(), response->get_query());
	set_cgi_path(response->get_path(), context);
	run_cgi(request, context, response->get_path());
	return (set_output_in_response_body(response));
}
