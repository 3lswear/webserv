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
	Response		_response;
	ServerConfig	*_config;

private:
	int		_ret;
	int		_fd;
	unsigned int		_sended;
	char	*_buff;


	std::string	_bodyToSend;
	std::string	_headerToSend;
	std::string	_toSend;
	std::map<std::string, std::string> _errorCode;

public:
	bool allRead;
	
public:
	Request						getRequest(void);
	Response					getResponse(void);
	std::string					getStrToSend(void);
	unsigned int				getCounter(void) const;
	void						setRawData(char *);
	void						setFd(int);
	int							getFd(void);


public:
	int							parseRequest(void);

	void						printClientInfo(void);

	bool						readyToSend(void);
	bool						allSended(void);
	bool						isChunked(void);
	int							sendResponse(int fd);
	int							sendData(int , std::string data);
	void						clear(void);
	void						increaseCounter(void);
	std::string					generateRespons(void);

	Client();
	Client(char *);
	Client(char *, ServerConfig *config);
	~Client();


};

#endif
