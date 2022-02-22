#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "webserv.hpp"
#include "ConfigException.hpp"
#include "parse.hpp"
 
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
	std::string					cgi_pass;
	ssize_t						clientBodySize;
};

struct serverListen
{
	std::string	ip;
	int 		port;
};

class ServerConfig
{
public:
	int			ret;
private:
	TOMLMap		*server;
	std::string	_serverName;
	std::string _host;
	int			_port;
	ssize_t			_clientBodySize;

	std::map<int, std::string>	_errorPages;
	std::vector<location *>		_locations;

	void	resetLocation(location *);
public:
	void	setServerName(std::string);
	void	setHost(std::string);
	void	setPort(int);
	void	setClientBodySize(int);
	void	setErrorPages(std::map<int, std::string>);
	void	setLocations(std::vector<location *>);
	void	setRoot(TOMLMap *);
	void	checkConfig(void);

	std::string					&getServerName(void);
	std::string					&getHost(void);
	int							&getPort(void);
	ssize_t						&getClientBodySize(void);
	std::vector<location *>		&getLocations(void);
	std::map<int, std::string>	&getErrorPages(void);
	TOMLMap						*getRoot(void);

public:
	ServerConfig();
	ServerConfig(TOMLMap	*root);
private:
	int	identify(TOMLMap::iterator it);
	int	putBodySizeLimit(toml_node *);
	int	putErrorPage(toml_node *);
	int	putHost(toml_node *);
	int	putName(toml_node *);
	int	putPort(toml_node *);
	int	putLocation(toml_node *);
	std::string	getTypestr(toml_node::e_type);
	std::string getWrongTypeErrorMSG(std::string field, toml_node::e_type expected, toml_node::e_type received);
	bool		checkFileAndDir(location *);
	int			isFile(std::string path);
	int			isDir(std::string path);


public:
	void	fillFields(void);
	void	printFields(void);


	~ServerConfig();
};

std::string	getDebugTime();



#endif
