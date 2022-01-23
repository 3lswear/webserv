#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	ret = 0;
}

ServerConfig::ServerConfig(TOMLMap *map)
{
	ret = 0;
	server = map;
	fillFields();
}

//--------------------------------------------------GET/SET---------------------------------------
std::string					ServerConfig::getServerName(void)
{
	return (_serverName);
}

std::string					ServerConfig::getHost(void)
{
	return (_host);
}

int							ServerConfig::getPort(void)
{
	return (_port);
}

int							ServerConfig::getClientBodySize(void)
{
	return (_clientBodySize);
}

std::vector<location>		ServerConfig::getLocations(void)
{
	return (_locations);
}

std::map<int, std::string>	ServerConfig::getErrorPages(void)
{
	return (_errorPages);
}

TOMLMap						*ServerConfig::getRoot(void)
{
    return (server);
}

void	ServerConfig::setServerName(std::string name)
{
	_serverName = name;
}

void	ServerConfig::setHost(std::string host)
{
	_host = host;
}

void	ServerConfig::setPort(int port)
{
	_port = port;
}

void	ServerConfig::setClientBodySize(int body)
{
	_clientBodySize = body;
}

void	ServerConfig::setErrorPages(std::map<int, std::string> pages)
{
	_errorPages  = pages;
}

void	ServerConfig::setLocations(std::vector<location> locations)
{
	_locations = locations;
}

void	ServerConfig::setRoot(TOMLMap * data)
{
	server = data;	
}


//--------------------------------------------------Parse-Config---------------------------------------

int	ServerConfig::putBodySizeLimit(toml_node *node)
{
	if (node->get_type() != toml_node::e_type::NUM)
		return (1);
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}
int	ServerConfig::putErrorPage(toml_node *node)
{
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}
int	ServerConfig::putHost(toml_node *node)
{
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}
int	ServerConfig::putName(toml_node *node)
{
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}
int	ServerConfig::putPort(toml_node *node)
{
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}
int	ServerConfig::putLocation(toml_node *node)
{
	std::cout << TURGUOISE << *node->toString() << ZERO_C << std::endl;
	return (0);
}

int		ServerConfig::identify(TOMLMap::iterator it)
{
	if (it->first == "body_size_limit")
		putBodySizeLimit(it->second);
	else if (it->first == "error_page")
		putErrorPage(it->second);
	else if (it->first == "host")
		putHost(it->second);
	else if (it->first == "location")
		putLocation(it->second);
	else if (it->first == "name")
		putName(it->second);
	else if (it->first == "port")
		putPort(it->second);
	else
		return (1);
	return (0);
}

void	ServerConfig::fillFields(void)
{

	TOMLMap::iterator	block;

	block = server->begin();
	while (block != server->end() && ret == 0)
	{
		std::cout << GREEN << block->first << ZERO_C << std::endl;
		ret = identify(block);
		++block;
	}
}

ServerConfig::~ServerConfig()
{
}