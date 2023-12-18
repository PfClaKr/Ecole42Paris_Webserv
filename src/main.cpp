#include "config/config_parser.hpp"
#include "config/config_tools.hpp"
#include "server_engine/server.hpp"
#include "server_engine/socket.hpp"
#include "http_response/response.hpp"
#include "http_request/request.hpp"

int main()
{
	Context c;
	parse_config(c, "src/config/default.conf");
	std::vector<Context *> servers = get_context_by_name(c, "server");
//	print_servers(servers);

	Socket s1,s2;
	try
	{
	 	s1.init_socket("0.0.0.0", "8080"); // <- c2 = s1
	 	s2.init_socket("0.0.0.0", "9090");
		// s1.init_socket(servers[0]->get_directive_by_key("host")[0], servers[0]->get_directive_by_key("listen")[0]);
		// s2.init_socket(servers[1]->get_directive_by_key("host")[0], servers[1]->get_directive_by_key("listen")[0]);
	}
	catch (Socket::SocketException &e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	Server s;
	//check multiple port
	s.set_server_set(s1, servers[0]);
	s.set_server_set(s2, servers[1]);
	s.init_server();
	return (0);
}
