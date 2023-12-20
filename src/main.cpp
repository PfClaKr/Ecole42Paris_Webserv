#include "config/config_parser.hpp"
#include "config/config_tools.hpp"
#include "server_engine/server.hpp"
#include "server_engine/socket.hpp"
#include "http_response/response.hpp"
#include "http_request/request.hpp"

int main(int argc, char **argv)
{
	#ifdef DEBUG
		std::cout << DARK_BLUE << "-------------DEBUG MODE-------------" << RESET << std::endl;
	#endif
	std::string conf;
	if (argc == 1)
		conf = "src/config/default.conf";
	else if (argc == 2)
		conf = argv[1];
	else
		return 1;
	Context c;
	if (parse_config(c, conf) == -1)
	{
		std::cerr << "parse error: check config file\n";
		return 1;
	}
	Server s;
	std::vector<Socket> sockets;
	std::vector<Context *> servers = get_context_by_name(c, "server");
	try
	{
		for (size_t i = 0; i < servers.size(); i++)
		{
			Socket so;
			so.init_socket(servers[i]->get_directive_by_key("host").front(), servers[i]->get_directive_by_key("listen").front());
			sockets.push_back(so);
			s.set_server_set(sockets[i], servers[i]);
		}
	}
	catch(Socket::SocketException& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}
	s.init_server();
	for (size_t i = 0; i < sockets.size(); i++)
		close(sockets[i].get_fd());
	return (0);
}
