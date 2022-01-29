#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"
#include "Autoindex.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Client
{
private:
	Request			_request;
	Response			_Response;
	ServerConfig	*_config;

private:
	int		_ret;
	int		_fd;
	char	*_buff;


	std::string	_bodyToSend;
	std::string	_ClientToSend;
	std::map<std::string, std::string> _errorCode;
	
public:
	Request						getRequest(void);
	Response						getResponse(void);
	void						setRawData(char *);
	void						setFd(int);
	int							getFd(void);

public:
	int							parseRequest(void);

	void						printClientInfo(void);
	
	int							sendResponse(int fd);
	int							sendData(int , std::string data);
	void						clear(void);

	Client();
	Client(char *);
	Client(char *, ServerConfig *config);
	~Client();


};

#endif
