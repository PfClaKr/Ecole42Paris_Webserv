#include "server.hpp"

int	init_server(Socket &sock)
{
	struct epoll_event epoll_ev;
	int	epoll_fd = epoll_create(MAX_PORTS);
	if (epoll_fd < 0)
	{
		perror("epoll"); //raise exception
		return (false);
	}
	for (size_t i = 0; i <= sock.quantity_listen; i++)
	{
		memset(&epoll_ev, 0, sizeof(epoll_ev));
		epoll_ev.events = EPOLLIN;
		epoll_ev.data.fd = sock.socketfd[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock.socketfd[i], &epoll_ev) < 0)
		{
			perror("epoll_ctl");
			return (false);
		}
	}
	//? have to see
	memset(&epoll_ev, 0, sizeof(epoll_ev));
	epoll_ev.events = EPOLLIN;
	epoll_ev.data.fd = 0;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &epoll_ev) < 0)
	{
		perror("epoll_ctl 2");
		return (false);
	}
	sock.epoll_fd = epoll_fd;
	return (true);
}

int	init_sockets(Socket &sock, std::string ip[], std::string port[])
{
	int	socketfd;
	int opt = 1;
	int	file_flag;
	struct sockaddr_in sa;

	for (int i = 0; i <= sock.quantity_listen; i++)
	{
		socketfd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketfd < 0)
		{
			perror("socket");
			return (false);
		}
		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		{
			perror("setsocket");
			return (false);
		}
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = inet_addr(ip[i].c_str());
		sa.sin_port = htons(stoi(port[i]));
		memset(sa.sin_zero, '\0', sizeof(sa.sin_zero));
		if (bind(socketfd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
		{
			perror("bind");
			return (false);
		}
		file_flag = fcntl(socketfd, F_GETFL, 0);
		if (file_flag < 0)
		{
			perror ("fcntl");
			return (false);
		}
		if (fcntl(socketfd, F_SETFL, file_flag | O_NONBLOCK) < 0)
		{
			perror("fcntl 2");
			return (false);
		}
		if (listen(socketfd, MAX_PORTS) < 0)
		{
			perror("listen");
			return (false);
		}
		sock.socketfd.push_back(socketfd);
	}
	return (true);
}

int	init_accept(Socket sock, int i)
{
	struct epoll_event epoll_ev;
	struct sockaddr in_addr;
	socklen_t in_addr_len = sizeof(in_addr);
	int	newfd;
	int	file_flag;

	newfd = accept(sock.socketfd[i], (struct sockaddr *)&in_addr, &in_addr_len);
	if (newfd < 0)
	{
		perror("accept");
		return (false);
	}
	file_flag = fcntl(newfd, F_GETFL, 0);
	if (file_flag < 0)
	{
		perror("fcntl");
		return (false);
	}
	if (fcntl(newfd, F_SETFL, file_flag | O_NONBLOCK) < 0)
	{
		perror("fcntl2");
		return (false);
	}
	memset(&epoll_ev, 0, sizeof(epoll_ev));
	epoll_ev.events = EPOLLIN;
	epoll_ev.data.fd = 0;
	std::cout << "Accept" << std::endl;
	if (epoll_ctl(sock.epoll_fd, EPOLL_CTL_ADD, newfd, &epoll_ev) < 0)
	{
		perror("epoll_ctl");
		return (false);
	}
	return (true);
}

int	socket_match(Socket sock, int event_fd)
{
	for (int i = 0; i <= sock.quantity_listen; i++)
	{
		if (event_fd == sock.socketfd[i])
			return (i);
	}
	return (-1);
}

int	listen_server(Socket sock)
{
	struct epoll_event events[MAX_PORTS];
	int	events_count;
	int	j = 0;

	std::cout << "wait the connection" << std::endl;
	events_count = epoll_wait(sock.epoll_fd, events, MAX_PORTS, -1);
	std::cout << "events_count :" << events_count << std::endl;
	if (events_count < 0)
	{
		perror("epoll_wait");
		return (false);
	}
	for (int i = 0; i < events_count; i++)
	{
		if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || (!(events[i].events & EPOLLIN)))
		{
			std::cerr << "epoll error" << std::endl;
			close(events[i].data.fd);
			return (true);
		}
		else if (socket_match(sock, events[i].data.fd) != -1)
		{
			if (init_accept(sock, i) == false)
				return (false);
		}
		else
		{
			int str_len;
			int client_fd = events[i].data.fd;
			char data[4096];
			str_len = read(client_fd, &data, sizeof(data));

			if (str_len == 0)
			{
				std::cout << "Disconnect" << std::endl;
				close(client_fd);
				epoll_ctl(sock.epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
			}
			else
				std::cout << "Recieved data :" << data << std::endl;
		}
	}
	return (true);
}

int	main()
{
	Socket sock;
	std::string count;
	std::cout << "give me count of listen" << std::endl;
	std::getline(std::cin, count);
	sock.quantity_listen = stoi(count) - 1;
	std::string port[sock.quantity_listen + 1];
	std::string ip[sock.quantity_listen + 1];
	for (int i = 0; i <= sock.quantity_listen ; i++)
	{
		std::cout << "give me ip adrr" << std::endl;
		std::getline(std::cin, ip[i]);
		std::cout << "give me port num" << std::endl;
		std::getline(std::cin, port[i]);
	}
	init_sockets(sock, ip, port);
	init_server(sock);
	std::cout << "Server starting" << std::endl;
	while (listen_server(sock) != false)
		;
	std::cout << "Server closed" << std::endl;
}