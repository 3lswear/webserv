#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "webserv.hpp"

struct location
{
	std::string					location;
	std::string					root;
	std::string					directoryFile;
	std::string					uploadDir;
	bool						autoindex;
	bool						uploadAccept;
	std::vector<std::string>	methods;
	std::map<int, std::string>	redirect;
};

class ServerConfig
{
private:
	// TOMLMap		*_root;
	std::string	_serverName;
	std::string _host;
	int			_port;
	int			_clientBodySize;

	std::map<int, std::string>	_errorPages;
	std::vector<location>		_locations;

public:
	void	setServerName(std::string);
	void	setHost(std::string);
	void	setPort(int);
	void	setClientBodySize(int);
	void	setErrorPages(std::map<int, std::string>);
	void	setLocations(std::vector<location>);
	// void	setRoot(TOMLMap *);

	std::string					getServerName(void);
	std::string					getHost(void);
	int							getPort(void);
	int							getClientBodySize(void);
	std::vector<location>		getLocations(void);
	std::map<int, std::string>	getErrorPages(void);
	// TOMLMap						*getRoot(void);

public:
	ServerConfig();
	// ServerConfig(TOMLMap	*root);
	
	void	fillFields(void);
	
	~ServerConfig();
};




#endif