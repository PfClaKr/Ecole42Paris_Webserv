#ifndef SERVER_HPP
# define SERVER_HPP

#include "socket.hpp"
#include "../config/config.hpp"
#include "../http_request/request.hpp"
#include "../http_response/response.hpp"

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

class Server
{
	public:
		int epoll_fd;
		int	is_up;
		std::vector<std::pair<Socket, Context *>> server_set;
		std::map<int, std::pair<Socket, Context *>> request_set;
		void set_server_set(Socket s, Context *c);

		int		init_server();
		int		accept_new_connection(int event_fd);
		void	init_epoll();
		void	init_epoll_wait();
		void	handle_epoll_events(int fd, epoll_event *epoll_ev);
		void	init_request(int event_fd, epoll_event *epoll_ev);
		void	init_response(int event_fd, epoll_event *epoll_ev);
		std::string recv_request(int fd);
};

#endif