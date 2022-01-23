#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
}

// ServerConfig::ServerConfig(TOMLMap *map)
// {
// 	_root = map;
// }

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

// TOMLMap						ServerConfig::*getRoot(void)
// {
//     return (this->_root);
// }

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

// void	ServerConfig::setRoot(TOMLMap * data)
// {
// 	_root = data;	
// }


void	ServerConfig::fillFields(void)
{
	// TOMLMap	*tmp = _root;
	// TOMLMap::iterator	it;

	// it = tmp->begin();

	// while (it != tmp->end())
	// {
	// 	std::cout << it->first << std::endl;
	// }
	
}

ServerConfig::~ServerConfig()
{
}