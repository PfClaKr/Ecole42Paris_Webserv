#ifndef SOCKET_HPP
# define SOCKET_HPP

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
#include "../config/colors.hpp"
#define MAX_EVENTS 512


class Socket
{
	private:
		int	fd;
		struct sockaddr_in addr;
		socklen_t socklen;
		void init_socket_address(std::string host, std::string port);
		void init_socket_option();
		void init_socket_bind();
		void init_socket_fd_nonblocking();
	public:
		Socket(void){ 
			fd = socket(AF_INET, SOCK_STREAM, 0);
			socklen = sizeof(addr);
		}
		void init_socket(std::string host, std::string port);

		int	get_fd();

		class SocketException : public std::exception
		{
			public:
				const char *what(void) const throw();
		};
};

#endif