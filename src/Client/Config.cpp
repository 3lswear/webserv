#include "Config.hpp"

Config::Config()
{
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

location    *Config::getLocation(std::vector<location *> &arr, std::string &URI)
{
	int     max = 0;
	int		len = 0;
	location            *tmp;
	std::vector<location *>::iterator   it;
	std::map<int, location *>	compare;

	it = arr.begin();
	while (it != arr.end())
	{
		tmp = *it;
		if (tmp->location == URI)
			return (tmp);
		len = calcLen(tmp->location, URI);
		if (compare.find(len) == compare.end())
			compare[len] = tmp;
		if (max < len)
			max = len;
		it++;
	}
	return (compare[max]);
	
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