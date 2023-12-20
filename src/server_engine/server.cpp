#include "server.hpp"

void	add_fd_in_epoll(int epoll_fd, int fd, uint32_t opt)
{
	struct epoll_event epoll_ev;

	epoll_ev.events = opt;
	epoll_ev.data.fd = fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &epoll_ev))
	{
		close(epoll_fd);
		throw (Server::epollException());
	}
}

int find_pair_by_key(std::vector<std::pair<Socket, Context *> > &pair, int &key)
{
	for (unsigned long i = 0; i < pair.size(); i++)
	{
		if (pair[i].first.get_fd() == key)
			return (i);
	}
	return (-1);
}

void	Server::init_epoll()
{
	this->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	size_t size = this->server_set.size();
	for (size_t i = 0; i < size; i++)
		add_fd_in_epoll(this->epoll_fd, this->server_set[i].first.get_fd(), EPOLLIN);
	#ifdef DEBUG
		std::cout << DARK_BLUE << "Number of servers: " << this->server_set.size() << RESET << std::endl;
	#endif
}

int	Server::accept_new_connection(int event_fd)
{
	// struct epoll_event epoll_ev;
	struct sockaddr_in addr;
	socklen_t socklen = sizeof(addr);

	int	new_fd = accept(event_fd, (struct sockaddr *)&addr, &socklen);

	std::cout << CYAN << "New connections arrived !" << RESET << std::endl;
	#ifdef DEBUG
		std::cout << DARK_BLUE << "in accept fd: " << new_fd << RESET << std::endl;
	#endif

	if (new_fd < 0)
		throw("accept");
	if (fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
		throw("fcntl accept");
	return (new_fd);
}

void	remove_fd_in_epoll(int epoll_fd, int fd, epoll_event *epoll_ev)
{
	std::cout << "Connection closed..." << std::endl;
	#ifdef DEBUG
		std::cout << DARK_BLUE << "closed fd: " << fd << RESET << std::endl;
	#endif
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, epoll_ev);
	close(fd);
}

std::string Server::recv_request(int fd)
{
	int	read_size = 0;
	char buf[2048];

	memset(&buf, 0, sizeof(buf));
	read_size = recv(fd, buf, sizeof(buf) - 1, MSG_DONTWAIT);

	#ifdef DEBUG
		std::cout << DARK_BLUE << "request fd: " << fd << RESET << std::endl;
	#endif
	std::cout << "Request :" << std::endl;
	std::cout << YELLOW << buf << RESET << std::endl;

	if (read_size == 0)
		return (""); //connection closed client fd remove
	else if (read_size == -1)
		throw (Socket::SocketException());
	return (buf);
}

void	Server::process_split_request(int event_fd, epoll_event *epoll_ev)
{
	//have to timeout?
	std::string data = recv_request(event_fd);
	this->response_set.first.body += data;
	this->check_split_request();
	if (!this->split_request)
	{
		epoll_ev->events = EPOLLOUT;
		epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, epoll_ev);
	}
}

void	Server::check_split_request()
{
	Request request = this->response_set.first;
	int	content_length = std::atoi(request.header["content-length"].c_str());
	#ifdef DEBUG
		std::cout << DARK_BLUE << "===================check split request==================" << std::endl;
		std::cout << "header length : " << request.header["content-length"] << std::endl;
		std::cout << "cotent_length : " << content_length << std::endl;
		std::cout << "body length : " << request.body.length() << std::endl;
	#endif
	if (request.body.length() < (unsigned long) content_length)
	{
		this->split_request = true;
		return ;
	}
	this->split_request = false;
}

void	Server::init_request(int event_fd, epoll_event *epoll_ev)
{
	if (this->split_request)
		return (process_split_request(event_fd, epoll_ev));
	Request request;
	std::string data = recv_request(event_fd);

	if (parse_http_request(request, data) == 400)
	{
		#ifdef DEBUG
			print_http_request(request);
		#endif
		this->response_set = std::make_pair(request, this->request_set[event_fd]);
		throw (Request::parsingException());
	}
	this->response_set = std::make_pair(request, this->request_set[event_fd]);
	this->check_split_request();
	#ifdef DEBUG
		print_http_request(request);
		std::cout << DARK_BLUE << "is_split request? fd is -> " << event_fd << ": ";
		this->split_request == true ? std::cout << "yes" : std::cout << "no";
		std::cout << RESET << std::endl;
	#endif
	if (!this->split_request)
	{
		epoll_ev->events = EPOLLOUT;
		epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, epoll_ev);
	}
}

void	Server::send_response(Response &response, int fd)
{
	int	send_size;
	send_size = send(fd, response.get_ready_to_send().c_str(), response.get_ready_to_send().size(), 0);

	#ifdef DEBUG
		std::cout << DARK_BLUE << "response fd: " << fd << RESET << std::endl;
	#endif
	std::cout << "Response :" << std::endl;
	std::cout << GREEN << response.get_ready_to_send() << RESET << std::endl;

	if (send_size <= 0)
		throw (Socket::SocketException());
}

int	Server::is_keep_alive()
{
	std::string config = this->response_set.first.header["connection"];
	if (config == "keep-alive")
		return (true);
	return (false);
}

void	Server::init_response(int event_fd, epoll_event *epoll_ev)
{
	Response response;

	response.make_http_response(response_set);
	send_response(response, event_fd);
	if (!is_keep_alive())
		remove_fd_in_epoll(this->epoll_fd, event_fd, epoll_ev);
	else
	{
		response_set.first.clear_value();
		epoll_ev->events = EPOLLIN;
		epoll_ctl(this->epoll_fd, EPOLL_CTL_MOD, event_fd, epoll_ev);
	}
}

void	Server::handle_epoll_events(int event_fd, epoll_event *epoll_ev)
{
	int event;
	event = find_pair_by_key(this->server_set, event_fd);
	try
	{
		if (event >= 0) //new connection arrive
		{
			int accept_fd = this->accept_new_connection(event_fd);
			add_fd_in_epoll(this->epoll_fd, accept_fd, EPOLLIN | EPOLLRDHUP);
			request_set[accept_fd] = server_set[event].second;
		}
		else if (epoll_ev->events & EPOLLRDHUP) //connection closed or half-close
			remove_fd_in_epoll(this->epoll_fd, event_fd, epoll_ev);
		else if (epoll_ev->events & EPOLLIN) //data came by registered connection
			init_request(event_fd, epoll_ev);
		else if (epoll_ev->events & EPOLLOUT) //ready to response
			init_response(event_fd, epoll_ev);
	}
	catch (Socket::SocketException &e)
	{
		std::cerr << e.what() << std::endl;
		remove_fd_in_epoll(this->epoll_fd, event_fd, epoll_ev);
	}
	catch (Request::parsingException &e)
	{
		Response response;
		response.make_error_response(response_set, BAD_REQUEST);
		send_response(response, event_fd);
		remove_fd_in_epoll(this->epoll_fd, event_fd, epoll_ev);
	}
}

void	Server::init_epoll_wait()
{
	this->is_up = true;
	int ev_count;
	struct epoll_event epoll_ev[MAX_EVENTS];

	memset(&epoll_ev, 0, sizeof(epoll_ev));
	while (is_up)
	{
		ev_count = epoll_wait(this->epoll_fd, epoll_ev, this->server_set.size(), -1);
		for (int i = 0; i < ev_count && is_up; i++)
			this->handle_epoll_events(epoll_ev[i].data.fd, &epoll_ev[i]);
	}
}

int Server::init_server()
{
	#ifdef DEBUG
		std::cout << DARK_BLUE << "----------init-server----------" << std::endl;
	#endif
	try
	{
		this->init_epoll();
		this->init_epoll_wait();
	}
	catch (std::exception())
	{
		return -1;
	}
	return 0;
}

void	Server::set_server_set(Socket s, Context *c)
{
	std::pair<Socket, Context *> ret = std::make_pair(s, c);
	this->server_set.push_back(ret);
}

Server::Server()
{
	this->split_request = false;
	this->epoll_fd = -1;
}

Server::Server(const Server &ref)
{
	(void)ref;
}

Server::~Server()
{
}

// int	main()
// {
// 	Server server;
// 	Socket s1, s2;
// 	Context c1, c2;

// 	s1.init_socket("0.0.0.0", "8080"); // <- c2 = s1
// 	s2.init_socket("0.0.0.0", "9090");
// 	server.set_server_set(s1, c1);
// 	server.set_server_set(s2, c2);
// 	server.init_server();
// }
