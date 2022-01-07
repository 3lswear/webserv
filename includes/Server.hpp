#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"

#define BUFFSIZE 1024

class Header;

class Server
{
private:
	int	_port;
	int	_serverSocket;
	int	_clientSocket;
	// int	_bufferSize;
	struct sockaddr_in _addres;
	std::string _ip;

private:
	void checkError(int fd, std::string str);
	void sendFile(std::string str);
	void sendHeader(Header head);
	void sendRespons(Header head);

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