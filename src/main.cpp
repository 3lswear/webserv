#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"

#ifdef DEBUG
#define DBOUT std::cerr << TURQ
#define ENDL RESET << std::endl
#else
#define DBOUT 0 && std::cerr
#define ENDL RESET << std::endl
#endif

int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server	server;
	
	 server.readConfig(); 
	 server.setupConfig();
	 server.start();
	//  server.end();


	return (0);
}
