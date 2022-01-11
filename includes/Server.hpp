#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"

#define BUFFSIZE	1024
#define MAX_CLIENT	1000

class Header;

class Server
{
private:
	int	_port;
	int	_epolfd;
	int	_client;
	struct epoll_event	_events[MAX_CLIENT];
	struct sockaddr_in _addres;
	std::string _ip;

private:
	void checkError(int fd, std::string str);
	void sendFile(std::string str);
	void sendHeader(Header head, int);
	void sendRespons(Header head, int);
	void setNonblocking(int fd);
	void newConnection(int fd);
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