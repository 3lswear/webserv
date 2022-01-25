#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server	server;
	
	 server.readConfig(); 
	 server.setupConfig();
	 server.start();


	return (0);
}
