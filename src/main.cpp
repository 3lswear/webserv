#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"


int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server	server;
	
	 server.readConfig(argv[1]); 
	 server.setupConfig();
	 server.start();
	 server.end();


	return (0);
}
