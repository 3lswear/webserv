#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"

#define BUFFSIZE 1024

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
	void sendHeader(void);
	void printRed(std::string str);
	void printYellow(std::string str);
	void printBlue(std::string str);
	void printPink(std::string str);
	void printGreen(std::string str);
	void printTurguoise(std::string str);

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