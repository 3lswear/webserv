#ifndef HEADER_HPP
# define HEADER_HPP

#include "webserv.hpp"
#include "HeaderHandl.hpp"
#include "Autoindex.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Respons.hpp"

class Header
{
private:
	Request			_request;
	Respons			_respons;
	ServerConfig	*_config;

private:
	int		_ret;
	int		_fd;
	char	*_buff;


	std::string	_bodyToSend;
	std::string	_headerToSend;
	std::map<std::string, std::string> _errorCode;
	
public:
	Request						getRequest(void);
	Respons						getRespons(void);
	void						setRawData(char *);
	void						setFd(int);
	int							getFd(void);

public:
	int							parseRequest(void);

	void						printHeaderInfo(void);
	
	int							sendRespons(int fd);
	int							sendData(int , std::string data);
	void						clear(void);

	Header();
	Header(char *);
	Header(char *, ServerConfig *config);
	~Header();


};

#endif