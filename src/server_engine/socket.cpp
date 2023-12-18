#include "socket.hpp"

void	Socket::init_socket_address(std::string host, std::string port)
{
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(stoi(port));
	memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
}

void	Socket::init_socket_option()
{
	int	opt = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		throw(Socket::SocketException());
}

void	Socket::init_socket_bind()
{
	if (bind(fd, (struct sockaddr *)&this->addr, this->socklen) < 0)
		throw(Socket::SocketException());
}

void	Socket::init_socket_fd_nonblocking()
{
	int	fd_flag = fcntl(this->fd, F_GETFL, 0);
	if (fd_flag < 0)
		throw(Socket::SocketException());
	if (fcntl(this->fd, F_SETFL, O_NONBLOCK) < 0)
		throw(Socket::SocketException());
}

void	Socket::init_socket(std::string host, std::string port)
{
	this->init_socket_address(host, port);
	this->init_socket_option();
	this->init_socket_bind();
	this->init_socket_fd_nonblocking();

	if (listen(this->fd, MAX_EVENTS) < 0)
		throw(Socket::SocketException());
	std::cout << "Setting socket " << host << ":" << port << "..." << std::endl;
}

int	Socket::get_fd()
{
	return (this->fd);
}

const char *Socket::SocketException::what() const throw()
{
	std::cout << RED << "Errors in the Socket setting" << RESET << std::endl;
	return (strerror(errno));
}

Socket::Socket()
{
	fd = socket(AF_INET, SOCK_STREAM, 0);
	socklen = sizeof(addr);
}

Socket::Socket(const Socket &ref)
{
	this->fd = ref.fd;
	this->addr = ref.addr;
	this->socklen =ref.socklen;
}

Socket::~Socket()
{
}