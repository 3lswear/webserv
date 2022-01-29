#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "ServerConfig.hpp"
#include "webserv.hpp"


class Request
{
private:
	char		*_data;
	
	int			_ret;
	int			_row;

	std::string _URI;
	std::string	_body;
	std::string	_host;
    std::string _query;
	std::string _method;
    std::string _fullURI;
    std::string _version;
	std::string	_location;
	std::map<std::string, std::string> _ClientField;

	ServerConfig	*_config;

public:
	std::string					getURI(void);
	std::string					getBody(void);
	std::string					getHost(void);
	std::string					getQuery(void);
	std::string					getMethod(void);
	std::string					getFullUri(void);
	std::string					getVersion(void);
	std::string					getLocation(void);
	ServerConfig				*getConfig(void);
	int							getCode(void);
	std::map<std::string, std::string>	getClientFields(void);

	void						setConfig(ServerConfig *config);
	void						setData(char *);
	void						setData(char *, ServerConfig *);

public:

	Request();
	Request(char *str);
	int							parseStartLine(std::string);
	int							parseClientfield(std::string);
	int							parseRequest(void);
	void						parseURI(std::string);
	void						printClientInfo(void);

	bool						badCode(int);
	int							isDir(std::string path);
	int							isFile(std::string path);
	bool						autoindexOn(void);
	void						copyFromMap(void);
	void						clear(void);

	~Request();
};



#endif