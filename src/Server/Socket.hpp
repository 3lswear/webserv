#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"

class Socket
{
private:
    int                 _socketFd;
    socklen_t           _addrlen;
	struct sockaddr_in  _addres;

    Socket();
    
public:
    Socket(int domain, int type, int protocol, int port, std::string ip);
    int getFd(void);
    struct sockaddr_in getAddres(void);
    socklen_t getAddlen(void);
    struct sockaddr *getAddr(void);
    socklen_t *getLen(void);
    void setSocketFd(int);
    void setAddres(struct sockaddr_in);
    int bindingSocket(void);
    int listeningSocket(int nbr);
    int init(int nbr);
    void checkError(int fd, std::string str);

	struct serverListen min_config;
	int port;
	std::string ip;

    ~Socket();
};



#endif
