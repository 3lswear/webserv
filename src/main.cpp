#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"
#include "ConfigException.hpp"


int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server	server;
	char 	*path = (char *)"config/real.toml";

	try
	{
		if (argv[1] != NULL)
			server.readConfig(argv[1]);
		else
			server.readConfig(path);
		server.start();
	}
	catch(const ConfigException& e)
	{
		server.end();
		std::cerr << RED << e.getMessage() << '\n' << ENDL;
	}
	
}
