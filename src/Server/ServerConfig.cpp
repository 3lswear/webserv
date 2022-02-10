#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	ret = 0;
	_clientBodySize = 0;
	_port = 0;
}

ServerConfig::ServerConfig(TOMLMap *map)
{
	ret = 0;
	server = map;
	_clientBodySize = 0;
	_port = 0;

	fillFields();
}

//--------------------------------------------------GET/SET---------------------------------------
std::string					&ServerConfig::getServerName(void)
{
	return (_serverName);
}

std::string					&ServerConfig::getHost(void)
{
	return (_host);
}

int							&ServerConfig::getPort(void)
{
	return (_port);
}

int							&ServerConfig::getClientBodySize(void)
{
	return (_clientBodySize);
}

std::vector<location *>		&ServerConfig::getLocations(void)
{
	return (_locations);
}

std::map<int, std::string>	&ServerConfig::getErrorPages(void)
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

void	ServerConfig::setLocations(std::vector<location *> locations)
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
	if (node->get_type() != toml_node::NUM)
		return (1);
	_clientBodySize = node->getNum();
	return (0);
}
int	ServerConfig::putErrorPage(toml_node *node)
{
	if (node->get_type() != toml_node::MAP)
		return (1);
	TOMLMap	*map = node->getMap();
	TOMLMap::iterator it;
	std::string s;

	it = map->begin();
	while (it != map->end())
	{
		if (it->second->get_type() != toml_node::STRING)
			return (1);
		s = it->first;
		_errorPages[atoi(s.c_str())] = *it->second->getString();
		++it;
	}
	return (0);
}
int	ServerConfig::putHost(toml_node *node)
{
	if (node->get_type() != toml_node::STRING)
		return (1);
	_host = *node->getString();
	return (0);
}
int	ServerConfig::putName(toml_node *node)
{
	if (node->get_type() != toml_node::STRING)
		return (1);
	_serverName = *node->getString();
	return (0);
}
int	ServerConfig::putPort(toml_node *node)
{
	if (node->get_type() != toml_node::NUM)
		return (1);
	_port = node->getNum();
	return (0);
}

void	ServerConfig::resetLocation(location *tmp)
{
	tmp->autoindex = false;
	tmp->uploadAccept = false;
	tmp->clientBodySize = 0;
}
int	ServerConfig::putLocation(toml_node *node)
{
	if (node->get_type() != toml_node::MAPARRAY)
		return (1);
	
	TOMLMapArray *arr = node->getMapArray();
	TOMLMapArray::iterator	it = arr->begin();
	TOMLMap		*map;
	TOMLMap::iterator		it1;
	location	*tmp;
	TOMLArray::iterator			it2;
	TOMLArray	Array;

	std::string	str;
	while (it != arr->end())
	{
		map = *it;
		it1 = map->begin();
		tmp = new location;
		resetLocation(tmp);
		while (it1 != map->end())
		{
			if (it1->first == "location")
			{
				if (it1->second->get_type() != toml_node::STRING)
					return (1);
				tmp->location = *it1->second->getString();
			}
			else if (it1->first == "root")
			{
				if (it1->second->get_type() != toml_node::STRING)
					return (1);
				tmp->root = *it1->second->getString();
			}
			else if (it1->first == "autoindex")
			{
				if (it1->second->get_type() != toml_node::BOOL)
					return (1);
				tmp->autoindex = it1->second->getBool();
			}
			else if (it1->first == "upload_accept")
			{
				if (it1->second->get_type() != toml_node::BOOL)
					return (1);
				tmp->uploadAccept = it1->second->getBool();
			}
			else if (it1->first == "upload_dir")
			{
				if (it1->second->get_type() != toml_node::STRING)
					return (1);
				tmp->uploadDir = *it1->second->getString();	
			}
			else if (it1->first == "cgi_pass")
			{
				if (it1->second->get_type() != toml_node::STRING)
					return (1);
				tmp->cgi_pass = *it1->second->getString();	
			}
			else if (it1->first == "body_size_limit")
			{
				if (node->get_type() != toml_node::NUM)
					return (1);
				tmp->clientBodySize = it1->second->getNum();
			}
			else if (it1->first == "directory_file")
			{
				if (it1->second->get_type() != toml_node::STRING)
					return (1);
				tmp->directoryFile = *it1->second->getString();	
			}
			else if (it1->first == "methods")
			{
				if (it1->second->get_type() != toml_node::ARRAY)
					return (1);
				Array = *it1->second->getArray();
				it2 = Array.begin();
				while (it2 != Array.end())
				{
					if ((*it2)->get_type() != toml_node::STRING)
						return (1);
					tmp->methods.push_back(*((*it2)->getString()));
					++it2;
				}

			}
			else if (it1->first == "redirect")
			{
				if (it1->second->get_type() != toml_node::ARRAY)
					return (1);
				Array = *it1->second->getArray();
				it2 = Array.begin();
				str = *(*it2)->getString();
				++it2;
				tmp->redirect.insert(std::make_pair(atoi(str.c_str()), *(*it2)->getString()));
			}
			else
				std::cout << RED << it1->first << ZERO_C << std::endl;
			it1++;
		}
		_locations.push_back(tmp);
		it++;
	}
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
		ret = identify(block);
		++block;
	}
}

void	ServerConfig::printFields(void)
{
	std::vector<location *>::iterator it;
	std::map<int, std::string>::iterator it1;
	std::vector<std::string>::iterator	it2;
	std::map<int, std::string>::iterator it3;

	it1 = _errorPages.begin();
	it = _locations.begin();
	std::cout << RED << "-------------------------Server-Start----------------------------------\n" << ZERO_C;
	std::cout << GREEN << "name" << " " << BLUE << _serverName << std::endl;
	std::cout << GREEN << "host" << " " << BLUE << _host << std::endl;
	std::cout << GREEN << "port" << " " << BLUE << _port << std::endl;
	std::cout << GREEN << "client_body_size" <<  " " << BLUE << _clientBodySize << std::endl;
	std::cout << GREEN << "location" << std::endl;
	while (it != _locations.end())
	{
		std::cout << PINK << "------------------------------------------------\n";
		std::cout << YELLOW << "location " << BLUE << (*it)->location <<std::endl;
		std::cout << YELLOW << "root " << BLUE << (*it)->root <<std::endl;
		std::cout << YELLOW << "directoryFile " << BLUE << (*it)->directoryFile <<std::endl;
		std::cout << YELLOW << "uploadDir " << BLUE << (*it)->uploadDir <<std::endl;
		std::cout << YELLOW << "autoindex " << BLUE << (*it)->autoindex <<std::endl;
		std::cout << YELLOW << "uploadAccept " << BLUE << (*it)->uploadAccept <<std::endl;
		std::cout << YELLOW << "cgi_pass " << BLUE << (*it)->cgi_pass <<std::endl;
		std::cout << YELLOW << "client_body_size " << BLUE << (*it)->clientBodySize <<std::endl;
		std::cout << YELLOW << "methods " << std::endl;
		it2 = (*it)->methods.begin();
		while (it2 != (*it)->methods.end())
		{
			std::cout << BLUE << *it2 << " ";
			it2++;
		}
		std::cout << std::endl;
		it3 = (*it)->redirect.begin();
		std::cout << YELLOW << "redirection" << RED << " " << it3->first << " "  << BLUE << it3->second << std::endl;
		++it;
		std::cout << PINK << "------------------------------------------------\n";
	}
	std::cout << GREEN << "error pages" << std::endl;
	while (it1 != _errorPages.end())
	{
		std::cout << YELLOW << it1->first << " "  << BLUE << it1->second << std::endl;
		++it1;
	}
	std::cout << RED << "-------------------------Server-End------------------------------------\n" << ZERO_C;
}

ServerConfig::~ServerConfig()
{
}