#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"
#include "Socket.hpp"
#include "parse.hpp"

class Client;

class Server
{
private:
	int	_port;
	int	_epoll_fd;
	int	_client;
	struct epoll_event	_events[MAX_CLIENT];
	struct sockaddr_in _addres;
	std::string _ip;
	std::vector<ServerConfig *>	_configs;
	/* void add_to_epoll_list(int fd, unsigned int ep_events); */

private:
	void checkError(int fd, std::string str);
	void sendFile(std::string str);
	void sendClient(Client head, int);
	void sendResponse(Client head, int);
	void setNonBlock(int fd);
	void add_to_epoll_list(int fd);
public:
	Server();
	Server(std::string path);

	void readConfig(void);
	void setupConfig(void);
	void start(void);
	void end(void);
	~Server();
};


#endif
