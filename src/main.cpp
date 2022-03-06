#include "webserv.hpp"
#include "parse.hpp"
#include "Server.hpp"


int	main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Server	server;
	char 	*path = (char *)"config/def.toml";

	try
	{
		if (argc > 2)
			throw std::domain_error("Invalid number of arguments");
		if (argv[1] != NULL)
			server.readConfig(argv[1]);
		else
			server.readConfig(path);
		server.run();
	}
	catch(const ConfigException& e)
	{
		std::cerr << RED << "\n" << WARNING << getDebugTime()
			<< FAIL << e.getMessage() << ENDL;
	}
	catch (std::domain_error &e)
	{
		std::cerr << FAIL << "FATAL: ";
		std::cerr << e.what() << RESET << std::endl;
	}

	
}
