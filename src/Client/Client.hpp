#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"
#include "Autoindex.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include <cstring>

class Client
{
private:
	Request			_request;
	Response		_response;
	ServerConfig	*_config;

private:
	int		_ret;
	int		_fd;
	struct	timeval	_time;
	unsigned int		_sended;
	char	*_buff;

	std::string	_stringBUF;
	std::string	_bodyToSend;
	std::string	_headerToSend;
	std::string	*_toSend;
	char *_to_send_char;
	std::map<std::string, std::string> _errorCode;

public:
	bool allRead;
	enum e_req_status
	{
		READING,
		WRITING,
		ENDED
	} req_status; 

	struct serverListen connected_to;

	int serverfd;
	size_t readn;
	size_t left;
	bool done;
	size_t response_len;
	
public:
	Request						getRequest(void);
	Response					getResponse(void);
	char 						*getStrToSend(void);
	unsigned int				getCounter(void) const;
	void						setRawData(char *);
	void						setRawData(std::string	&);
	void						setFd(int);
	int							getFd(void);
	unsigned int 				getRecvCounter(void) const;
	bool						isEmpty(void);
	bool						TimeToDie(void);
	void						updateTimeToDie(void);
	serverListen				getIpPort(void);
public:
	int							parseRequest(void);

	void						printClientInfo(void);

	bool						readyToSend(void);
	bool						allSended(void);
	bool						allRecved(void);
	bool						isChunked(void);
	int							sendResponse(int fd);
	int							sendData(int , std::string data);
	void						clear(void);
	void						increaseCounter(void);
	void						increaseRecvCounter(unsigned int n);
	std::string					generateRespons(void);
	/* std::string					generateRespons(serverListen &, std::vector<ServerConfig *> &); */
	std::string					generateRespons(std::vector<ServerConfig *> &);

	/* Client(); */
	Client(struct serverListen servers_config);
	Client(char *);
	Client(char *, ServerConfig *config);
	~Client();

};

#endif
