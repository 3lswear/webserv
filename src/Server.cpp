#include "webserv.hpp"
//----------------------------------------------Constructors-----------------------------------------------------------------------------------
Server::Server()
{
	_client = 0;

}

Server::Server(std::string path)
{
	(void)path;
	_client = 0;
}

//----------------------------------------------Send--------------------------------------------------------------------------------------------

//----------------------------------------------Configuration-----------------------------------------------------------------------------------
void	Server::readConfig(void)
{

}

void	Server::setupConfig(void)
{
	this->_ip = "127.0.0.1";
	this->_port = 8080;
}


void	Server::setNonblocking(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

void	Server::newConnection(int fd)
{
	struct epoll_event	ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = fd;

	epoll_ctl(_epolfd, EPOLL_CTL_ADD, fd, &ev);
	_client++;
}

void	Server::start(void)
{
	Socket		serverSocket(AF_INET, SOCK_STREAM, 0, _port);
 	char		buff[BUFFSIZE + 1] = {0};
	Header		header;
	int			fd_accept;
	int			code;

	checkError(serverSocket.init(MAX_CLIENT), "Socket init");
	fd_accept = accept(serverSocket.getSocketFd(),
		serverSocket.getSockaddr(), serverSocket.getSocklen());
	checkError(fd_accept, "Initialize client socket");
	checkError(recv(fd_accept, buff, BUFFSIZE, 0), "Receive msg from client");
	std::cout << TURGUOISE << "Receive Header" << ZERO_C << std::endl;
	header.setRawData(buff);
	code = header.parseRequest();
	header.printHeaderInfo();
	header.sendRespons(fd_accept);
	std::cout << BLUE << header.getReasonPhrase(code) << ZERO_C << std::endl;
	close(fd_accept);
	close(serverSocket.getSocketFd());
//-----------------------------------------------попытка добавить epoll------------------
	// Socket		serverSocket(AF_INET, SOCK_STREAM, 0, _port);
 	// char		buff[BUFFSIZE + 1] = {0};
	// Header		header[MAX_CLIENT];
	// int			fd;
	// int			n;
	// int			nfds;
	// int			_clientSocket;

	// nfds = 0;
	// n = 0;
	// _epolfd = epoll_create1(0);
	// checkError(serverSocket.init(MAX_CLIENT), "Socket init");
	// setNonblocking(serverSocket.getSocketFd());
	// setNonblocking(_epolfd);
	// while (true)
	// {
	// 	_clientSocket = accept(serverSocket.getSocketFd(),
	// 		serverSocket.getSockaddr(), serverSocket.getSocklen());
	// 	if (_clientSocket > 0)
	// 		newConnection(_clientSocket);
	// 	if (_client > 0)
	// 		nfds = epoll_wait(_epolfd, _events, MAX_CLIENT, -1);
	// 	while (n < nfds)
	// 	{
	// 		fd = _events[n].data.fd;
	// 		// std::cout << "n = " << n << " nfds = " << nfds << " fd = " << fd << " _clientSize = " << _client << " _clientSocket " << _clientSocket << std::endl;
	// 		checkError(recv(fd, buff, BUFFSIZE, 0), "Receive msg from client");
	// 		header[fd].setRequest(buff);
	// 		header[fd].parseRequest();
	// 		header[fd].printHeaderInfo();
	// 		sendRespons(header[fd], fd);
	// 		header[fd].clearHeader();
	// 		close(fd);
	// 		_client--;
	// 		n++;
	// 	}
	// 	n = 0;
	// 	nfds = 0;
	// 	_clientSocket = 0;
	// }
	// close(serverSocket.getSocketFd());
}



void	Server::end(void)
{

}
//----------------------------------------------Other------------------------------------------------------------------------------------------------
void	Server::checkError(int fd, std::string str)
{
	if (fd < 0)
	{
		std::cout << RED << "Server ERROR: " << str << ZERO_C << std::endl;
		exit(1);
	}
	else
		std::cout << GREEN << "Server SUCCESS: " << str << ZERO_C << std::endl;
}

Server::~Server()
{
}

