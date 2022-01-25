#include "Server.hpp"
#include <exception>

#define THREAD_NUM 100
#define MAX_EVENTS

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
	TOMLMap *root = parse();


	TOMLMap *map;
	TOMLMap::iterator it1;
	TOMLMapArray	*arr;
	TOMLMapArray::iterator	it;


	arr = root->find("server")->second->getMapArray();
	it = arr->begin();

	while (it != arr->end())
	{
		std::cout << BLUE << *it << std::endl;
		map = *it;

		it1 = map->begin();
		while (it1 != map->end())
		{
			std::cout << TURGUOISE << it1->first << it1->second << ZERO_C << std::endl;
			++it1;
		}
		

		++it;
	}
	

	

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

	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
	_client++;
}

void Server::add_to_epoll_list(int fd, unsigned int ep_events)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = ep_events;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event))
			throw std::logic_error("epoll add");

}

void	Server::start(void)
{

	Socket server_sock(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1");
	char buf[BUFFSIZE + 1] = {0};
	Header header[MAX_CLIENT];
	int fd;
	/* int n; */
	/* int nfds; */
	int client_sock;
	int code;
	int epoll_ret = 0;
	/* int epollfd; */


	struct epoll_event ev;

	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	/* ev.data.fd */ 

	/* unsigned int ep_events = EPOLLIN | EPOLLOUT | EPOLLET; */
	
	_epoll_fd = epoll_create1(0);
	checkError(server_sock.init(MAX_CLIENT), "Socket init");
	setNonblocking(server_sock.getSocketFd());
	setNonblocking(_epoll_fd);
	while (1)
	{
		client_sock = accept(server_sock.getSocketFd(),
				server_sock.getSockaddr(), server_sock.getSocklen());
		if (client_sock > 0)
			newConnection(client_sock);
		if (_client > 0)
			epoll_ret = epoll_wait(_epoll_fd, _events, MAX_CLIENT, -1);
		if (epoll_ret < 0)
			throw std::logic_error("epoll_ret");
		for (int i = 0; i < epoll_ret; i++)
		{
			fd = _events[i].data.fd;
			checkError(recv(fd, buf, BUFFSIZE, 0), "Receive msg from client");
			header[fd].setRawData(buf);
			code = header[fd].parseRequest();
			header[fd].printHeaderInfo();
			header[fd].printHeaderInfo();
			header[fd].sendRespons(fd);
			header[fd].clear();
			std::cout << BLUE << header[fd].getReasonPhrase(code) << ZERO_C << std::endl;
			close(fd);
			_client--;
		}
		epoll_ret = 0;

	}
	close(server_sock.getSocketFd());
	std::cerr << "end;" << std::endl;
	





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
	//






	/* Socket		serverSocket(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1"); */
 	/* char		buff[BUFFSIZE + 1] = {0}; */
	/* Header		header; */
	/* int			fd_accept; */
	/* int			code; */

	/* checkError(serverSocket.init(MAX_CLIENT), "Socket init"); */
	/* fd_accept = accept(serverSocket.getSocketFd(), */
	/* 	serverSocket.getSockaddr(), serverSocket.getSocklen()); */
	/* checkError(fd_accept, "Initialize client socket"); */
	/* checkError(recv(fd_accept, buff, BUFFSIZE, 0), "Receive msg from client"); */
	/* std::cout << TURGUOISE << "Receive Header" << ZERO_C << std::endl; */
	/* header.setRawData(buff); */
	/* code = header.parseRequest(); */
	/* header.printHeaderInfo(); */
	/* header.sendRespons(fd_accept); */
	/* std::cout << BLUE << header.getReasonPhrase(code) << ZERO_C << std::endl; */
	/* close(fd_accept); */
	/* close(serverSocket.getSocketFd()); */
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

