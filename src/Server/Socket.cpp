#include "Socket.hpp"

Socket::Socket()
{

}
Socket::Socket(int domain, int type, int protocol, int port, std::string ip)
{
    int opt = 1;
    _socketFd = socket(domain, type, protocol);
    checkError(_socketFd, "Initialize Server socket");
    checkError(setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "Set socket options");
    _addres.sin_family = domain;
    _addres.sin_port = htons(port);
    _addres.sin_addr.s_addr = inet_addr(ip.c_str());
    _addrlen = sizeof(_addres);

	min_config.ip = ip;
	min_config.port = port;
	this->port = port;
	this->ip = ip;
}

Socket::~Socket()
{

}

//-----------------------------------------------------------------GET/SET----------------------------
int Socket::getFd(void)
{
    return _socketFd;
}

struct sockaddr_in Socket::getAddres(void)
{
    return _addres;
}

struct sockaddr *Socket::getAddr(void)
{
    return((struct sockaddr *)&_addres);
}

socklen_t   *Socket::getLen(void)
{
    return((socklen_t *)&_addrlen);
}


void    Socket::setAddres(struct sockaddr_in addr)
{
    _addres = addr;
    _addrlen = sizeof(_addres);
}

socklen_t   Socket::getAddlen(void)
{
    return (_addrlen);
}

void    Socket::setSocketFd(int fd)
{
    _socketFd = fd;
}
//----------------------------------------------------------------Setting-------------------------------
int Socket::bindingSocket(void)
{
    int res;

    res = bind(_socketFd, (struct sockaddr *)&_addres, sizeof(_addres));
    /* checkError(res, "Bind Socket"); */
    return (res);
}

int Socket::listeningSocket(int nbr)
{
    int res;

    res = listen(_socketFd, nbr);
    /* checkError(res, "Listen Socket"); */
    return (res);
}

int Socket::init(int nbr)
{
    if (bindingSocket() < 0)
	{
		std::cerr << getDebugTime() << FAIL;
		perror("Bind");
		std::cerr << RESET;
		return (-1);
	}

    if (listeningSocket(nbr) < 0)
        return (-1);
    return (0);
}

//---------------------------------------------------------------ERROR----------------------------------

void	Socket::checkError(int fd, std::string str)
{
	if (fd < 0)
	{
		perror(str.c_str());
		exit(1);
	}
}
