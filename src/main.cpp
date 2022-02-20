#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"


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
		server.run();
	}
	catch(const ConfigException& e)
	{
		std::cerr << RED << "\n" << e.getMessage() << '\n' << ENDL;
	}
	catch (std::domain_error &e)
	{
		std::cerr << RED << "FATAL: ";
		std::cerr << e.what() << RESET << std::endl;
	}

	
}
