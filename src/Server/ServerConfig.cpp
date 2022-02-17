#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
{
	ret = 0;
	_clientBodySize = -1;
	_port = 0;
}

ServerConfig::ServerConfig(TOMLMap *map)
{
	ret = 0;
	server = map;
	_clientBodySize = -1;
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

ssize_t							&ServerConfig::getClientBodySize(void)
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
std::string	ServerConfig::getTypestr(toml_node::e_type type)
{
	if (type == toml_node::NUM)
		return ("NUM");
	else if (type == toml_node::ARRAY)
		return ("ARRAY");
	else if (type == toml_node::BOOL)
		return ("BOOL");
	else if (type == toml_node::STRING)
		return ("STRING");
	else if (type == toml_node::MAP)
		return ("MAP");
	else if (type == toml_node::MAPARRAY)
		return ("MAPARRAY");
	else
		return ("UNKNOWN");
}
std::string ServerConfig::getWrongTypeErrorMSG(std::string field, toml_node::e_type expected, toml_node::e_type received)
{
	std::string out = "Wrong type specified in " + field + " field, expected "
		+ getTypestr(expected) + ", but received " + getTypestr(received) + "!";
	return (out);
}
int	ServerConfig::putBodySizeLimit(toml_node *node)
{
	if (node->get_type() != toml_node::NUM)
		throw ConfigException(getWrongTypeErrorMSG("body_size_limit", toml_node::NUM, node->get_type()));
	_clientBodySize = node->getNum();
	if (_clientBodySize < 0)
		throw ConfigException("Invalid body_size_limit specified!");
	return (0);
}
int	ServerConfig::putErrorPage(toml_node *node)
{
	if (node->get_type() != toml_node::MAP)
		throw ConfigException(getWrongTypeErrorMSG("error_page", toml_node::MAP, node->get_type()));
	TOMLMap	*map = node->getMap();
	TOMLMap::iterator it;
	std::string s;

	it = map->begin();
	while (it != map->end())
	{
		if (it->second->get_type() != toml_node::STRING)
			throw ConfigException(getWrongTypeErrorMSG("error_page path", toml_node::STRING, node->get_type()));
		s = it->first;
		_errorPages[atoi(s.c_str())] = *it->second->getString();
		++it;
	}
	return (0);
}
int	ServerConfig::putHost(toml_node *node)
{
	if (node->get_type() != toml_node::STRING)
		throw ConfigException(getWrongTypeErrorMSG("host", toml_node::STRING, node->get_type()));
	_host = *node->getString();
	int	ret = inet_addr(_host.c_str());
	if (ret == -1)
		throw ConfigException("Invalid host specified!");
	return (0);
}
int	ServerConfig::putName(toml_node *node)
{
	if (node->get_type() != toml_node::STRING)
		throw ConfigException(getWrongTypeErrorMSG("server_name", toml_node::STRING, node->get_type()));
	_serverName = *node->getString();
	return (0);
}
int	ServerConfig::putPort(toml_node *node)
{
	if (node->get_type() != toml_node::NUM)
		throw ConfigException(getWrongTypeErrorMSG("server_name", toml_node::NUM, node->get_type()));
	_port = node->getNum();
	if (_port < 0 || _port > 65536)
		throw ConfigException("Invalid port specified!");
	return (0);
}

void	ServerConfig::resetLocation(location *tmp)
{
	tmp->autoindex = false;
	tmp->uploadAccept = false;
	tmp->clientBodySize = -1;
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
		for (;it1 != map->end(); it1++)
		{
			if (it1->first == "location")
			{
				if (it1->second->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("location", toml_node::STRING, it1->second->get_type()));
				tmp->location = *it1->second->getString();
			}
			else if (it1->first == "root")
			{
				if (it1->second->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("root", toml_node::STRING, it1->second->get_type()));
				tmp->root = *it1->second->getString();
			}
			else if (it1->first == "autoindex")
			{
				if (it1->second->get_type() != toml_node::BOOL)
					throw ConfigException(getWrongTypeErrorMSG("autoindex", toml_node::BOOL, it1->second->get_type()));
				tmp->autoindex = it1->second->getBool();
			}
			else if (it1->first == "upload_accept")
			{
				if (it1->second->get_type() != toml_node::BOOL)
					throw ConfigException(getWrongTypeErrorMSG("upload_accept", toml_node::BOOL, it1->second->get_type()));
				tmp->uploadAccept = it1->second->getBool();
			}
			else if (it1->first == "upload_dir")
			{
				if (it1->second->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("upload_dir", toml_node::STRING, it1->second->get_type()));
				tmp->uploadDir = *it1->second->getString();	
			}
			else if (it1->first == "cgi_pass")
			{
				if (it1->second->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("cgi_pass", toml_node::STRING, it1->second->get_type()));
				tmp->cgi_pass = *it1->second->getString();	
			}
			else if (it1->first == "body_size_limit")
			{
				if (it1->second->get_type() != toml_node::NUM)
					throw ConfigException(getWrongTypeErrorMSG("body_size_limit", toml_node::NUM, it1->second->get_type()));
				tmp->clientBodySize = it1->second->getNum();
			}
			else if (it1->first == "directory_file")
			{
				if (it1->second->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("directory_file", toml_node::STRING, it1->second->get_type()));
				tmp->directoryFile = *it1->second->getString();	
			}
			else if (it1->first == "methods")
			{
				if (it1->second->get_type() != toml_node::ARRAY)
					throw ConfigException(getWrongTypeErrorMSG("methods", toml_node::ARRAY, it1->second->get_type()));
				Array = *it1->second->getArray();
				it2 = Array.begin();
				while (it2 != Array.end())
				{
					if ((*it2)->get_type() != toml_node::STRING)
						throw ConfigException(getWrongTypeErrorMSG("methods elem", toml_node::STRING, (*it2)->get_type()));
					tmp->methods.push_back(*((*it2)->getString()));
					++it2;
				}
			}
			else if (it1->first == "redirect")
			{
				if (it1->second->get_type() != toml_node::ARRAY)
					throw ConfigException(getWrongTypeErrorMSG("redirect", toml_node::ARRAY, it1->second->get_type()));
				Array = *it1->second->getArray();
				if (Array.size() != 2)
					throw ConfigException("The redirect field specified the wrong number of arguments!");
				it2 = Array.begin();
				if ((*it2)->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("redirect elem", toml_node::STRING, (*it2)->get_type()));
				str = *(*it2)->getString();
				++it2;
				if ((*it2)->get_type() != toml_node::STRING)
					throw ConfigException(getWrongTypeErrorMSG("redirect elem", toml_node::STRING, (*it2)->get_type()));
				tmp->redirect.insert(std::make_pair(atoi(str.c_str()), *(*it2)->getString()));
			}
			else
				std::cerr << RED << "Warning: unknown parameter: "<< it1->first << ZERO_C << std::endl;
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
	checkConfig();
}

int	ServerConfig::isFile(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (-1);
		else if (s.st_mode & S_IFREG)
			return (0);
	}
	else
		return (-1);
	return (-1);
}

int	ServerConfig::isDir(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (0);
		else if (s.st_mode & S_IFREG)
			return (-1);
	}
	else
		return (-1);
	return (-1);
}

bool	ServerConfig::checkFileAndDir(location *loc)
{
	std::string	root = loc->root;
	std::string	upload_dir = loc->uploadDir;
	std::string	directory_file = loc->directoryFile;

	if (!root.empty())
	{
		if (isDir(root) != 0)
			throw ConfigException("Directory " + root + " not found!");
	}
	if (!upload_dir.empty())
	{
		if (isDir(upload_dir) != 0)
			throw ConfigException("Directory " + upload_dir + " not found!");
	}
	if (!directory_file.empty())
	{
		directory_file = root + "/" + directory_file;
		if (isFile(directory_file) != 0)
			throw ConfigException("File " + directory_file +  " not found!");
	}
	return (true);
}

void	ServerConfig::checkConfig(void)
{
	if (_host.empty())
		throw	ConfigException("Host field not set!");
	else if (_port == 0)
		throw	ConfigException("Port field not set!");
	std::vector<location *>::iterator	it;

	it = _locations.begin();
	location *tmp;
	while (it != _locations.end())
	{
		tmp = *it;
		if (tmp->location.empty())
			throw	ConfigException("Location field not set!");
		checkFileAndDir(tmp);
		it++;
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