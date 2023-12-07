#ifndef SERVER_HPP
# define SERVER_HPP

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
#define MAX_PORTS 512


class Socket
{
	public:
		int	quantity_listen;
		int	epoll_fd;
		std::vector<int> socketfd;
};

#endif