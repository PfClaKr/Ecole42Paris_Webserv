#ifndef SERVER_HPP
# define SERVER_HPP

#include "socket.hpp"
#include "../config/context.hpp"
#include "../http_request/request.hpp"
#include "../http_response/response.hpp"

class Server
{
	private:
		bool	split_request;
		int epoll_fd;
		std::vector<std::pair<Socket, Context *>> server_set;
		std::map<int, Context *> request_set;
		std::pair<Request, Context *> response_set;
		int		accept_new_connection(int event_fd);
		void	init_epoll();
		void	init_epoll_wait();
		void	handle_epoll_events(int fd, epoll_event *epoll_ev);
		void	init_request(int event_fd, epoll_event *epoll_ev);
		void	check_split_request();
		void	process_split_request(int event_fd, epoll_event *epoll_ev);
		void	init_response(int event_fd, epoll_event *epoll_ev);
		int		is_keep_alive();
		std::string recv_request(int fd);
		void	send_response(Response &response, int fd);
	public:
		int	is_up;
		void set_server_set(Socket s, Context *c);
		int		init_server();


		int		get_epoll_fd();
		std::vector<std::pair<Socket, Context *>> get_server_set();
		std::map<int, std::pair<Socket, Context *>> get_request_set();

		class epollException : public std::exception
		{
		public:
			virtual const char* what() const throw()
			{
				return "Epoll Error";
			}
		};
		class parsingException : public std::exception
		{
		public:
			virtual const char* what() const throw()
			{
				return "Parsing Error Exception";
			};
		};
};

#endif
