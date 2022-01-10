#ifndef SOCKET
#define SOCKET

#include "webserv.hpp"
class Socket
{
private:
    int                 _socketFd;
    socklen_t           _addrlen;
	struct sockaddr_in  _addres;

    
public:
    Socket();
    Socket(int domain, int type, int protocol, int port);
    int getSocketFd(void);
    struct sockaddr_in getAddres(void);
    socklen_t getAddlen(void);
    struct sockaddr *getSockaddr(void);
    socklen_t *getSocklen(void);
    void setSocketFd(int);
    void setAddres(struct sockaddr_in);
    int bindingSocket(void);
    int listeningSocket(int nbr);
    int init(int nbr);
    void checkError(int fd, std::string str);

    ~Socket();
};



#endif