#include "http_response/response.hpp"
#include "http_request/request.hpp"
#include "config/config.hpp"
#include "server_engine/server.hpp"
#include "server_engine/socket.hpp"

int main()
{
	Context c;
	std::vector<Context *> servers;
	parse_config(c, "config/default_config");
	get_servers(c, servers);
//	print_servers(servers);

	Socket s1,s2;
	s1.init_socket(servers[0]->directive["host"][0] , servers[0]->directive["listen"][0]);
	s2.init_socket(servers[1]->directive["host"][0], servers[1]->directive["listen"][0]);
	Server s;
	s.set_server_set(s1, servers[0]);
	s.set_server_set(s2, servers[1]);
	s.init_server();
	return (0);
}
