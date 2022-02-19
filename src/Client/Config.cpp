#include "Config.hpp"

Config::Config()
{
}


int	isFile(std::string path)
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

int	isDir(std::string path)
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

void	copyLocation(location *dest, location *src)
{
	dest->autoindex = src->autoindex;
	dest->cgi_pass = src->cgi_pass;
	dest->clientBodySize = src->clientBodySize;
	dest->directoryFile = src->directoryFile;
	dest->location = src->location;
	dest->methods = src->methods;
	dest->redirect = src->redirect;
	dest->root = src->root;
	dest->uploadAccept = src->uploadAccept;
	dest->uploadDir = src->uploadDir;
}

int			Config::calcLen(std::string &s1, std::string &s2)
{
	unsigned long	len = 0;
	while ((len < (s1.size() )) && (len < (s2.size())))
	{
		if (s1[len] != s2[len])
			break;
		len++;
	}
	return (len);
}

std::vector<location *> Config::getLocation(std::vector<location *> &arr, std::string &URI)
{
	unsigned int	tryLen = URI.size();
	std::string		tryLocation;
	location		*tmp;
	std::string		suffix;
	std::string		suffix1;
	std::vector<location *>::iterator   it;
	std::vector<location *> step_1;
	std::vector<location *> step_2;
	suffix = URI.substr(URI.rfind(".") + 1, URI.size() - URI.rfind("."));

	while (tryLen)
	{
		it = arr.begin();
		while (it != arr.end())
		{
			tmp = *it;
			tryLocation = URI.substr(0, tryLen);
			if (tmp->location == tryLocation)
				step_1.push_back(tmp);
			it++;
		}
		if (!step_1.empty())
			break;
		tryLen = URI.rfind("/", tryLen - 1);
	}
	if (!step_1.empty())
	{
		it = step_1.begin();
		tmp = *it;
		if (tmp->location == URI || tmp->location.size() > 1)
			step_2.push_back(tmp);
	}

	it = arr.begin();
	while (it != arr.end())
	{
		tmp = *it;
		if (tmp->location[0] == '*')
		{
			suffix1 = tmp->location.substr(2);
			if (suffix1 == suffix)
				step_2.push_back(tmp);
		}
		it++;
	}
	if (step_2.empty())
	{
		it = arr.begin();
		while (it != arr.end())
		{
			tmp = *it;
			if (tmp->location == "/")
				step_2.push_back(tmp);
			it++;
		}
	}
	return (step_2);
}

ServerConfig    *Config::getConfig(std::vector<ServerConfig *> &arr, Request &request, serverListen &data)
{
	ServerConfig    *tmp;
	std::vector<ServerConfig *>::iterator   it;
	std::vector<ServerConfig *>   step_1;
	std::vector<ServerConfig *>   step_2;

	it = arr.begin();
	while (it != arr.end())
	{
		tmp = *it;
		if (tmp->getPort() == data.port && tmp->getHost() == data.ip)
			step_1.push_back(tmp);
		it++;
	}
	if (step_1.size() == 1)
		return (step_1[0]);
	it = step_1.begin();
	while (it != step_1.end())
	{
		tmp = *it;
		if (!tmp->getServerName().empty())
			step_2.push_back(tmp);
		it++;
	}
	if (step_2.size() == 0)
		return (step_1[0]);
	it = step_2.begin();
	while (it != step_2.end())
	{
		tmp = *it;
		if (tmp->getServerName() == request.getHost())
			return (tmp);
		it++;
	}
	return (step_1[0]);    
}


Config::~Config()
{
}