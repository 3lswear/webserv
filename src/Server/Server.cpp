#include "Server.hpp"
#include <exception>
#include <cassert>
#include <cstdio>
#include <map>

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


	/* TOMLMap *map; */
	TOMLMap::iterator it1;
	TOMLMapArray	*arr;
	TOMLMapArray::iterator	it;


	arr = root->find("server")->second->getMapArray();
	it = arr->begin();

	/* while (it != arr->end()) */
	/* { */
	/* 	std::cout << BLUE << *it << std::endl; */
	/* 	map = *it; */

	/* 	it1 = map->begin(); */
	/* 	while (it1 != map->end()) */
	/* 	{ */
	/* 		std::cout << TURGUOISE << it1->first << it1->second << ZERO_C << std::endl; */
	/* 		++it1; */
	/* 	} */
		

	/* 	++it; */
	/* } */
	

	

}

void	Server::setupConfig(void)
{
	this->_ip = "127.0.0.1";
	this->_port = 8080;
}


void	Server::setNonBlock(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
}

void	Server::add_to_epoll_list(int fd)
{
	struct epoll_event	ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
	ev.data.fd = fd;

	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
	_client++;
	std::cerr << RED
		<< "add client_sock "
		<< fd
		<< " to epoll_list"
		<< RESET << std::endl;
}

/* void Server::add_to_epoll_list(int fd, unsigned int ep_events) */
/* { */
/* 	struct epoll_event event; */
/* 	event.data.fd = fd; */
/* 	event.events = ep_events; */
/* 	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &event)) */
/* 			throw std::logic_error("epoll add"); */

/* } */

void	Server::start(void)
{

	Socket server_sock(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1");
	char buf[BUFFSIZE + 1] = {0};
	Header header[MAX_CLIENT];
	std::map<int, Header> header_map;
	int fd;
	/* int n; */
	/* int nfds; */
	int client_sock;
	int status;
	int epoll_ret = 0;

	/* struct epoll_event ev; */

	/* ev.events = EPOLLIN | EPOLLOUT | EPOLLET; */
	/* ev.data.fd */ 

	/* unsigned int ep_events = EPOLLIN | EPOLLOUT | EPOLLET; */
	
	_epoll_fd = epoll_create1(0);
	checkError(server_sock.init(MAX_CLIENT), "Socket init");
	setNonBlock(server_sock.getSocketFd());
	setNonBlock(_epoll_fd);
	while (1)
	{
		/* std::cout << RED << "IN MAIN LOOP" << RESET << std::endl; */
		client_sock = accept(server_sock.getSocketFd(),
				server_sock.getSockaddr(), server_sock.getSocklen());
		if (client_sock > 0)
			add_to_epoll_list(client_sock);
		if (_client > 0)
			epoll_ret = epoll_wait(_epoll_fd, _events, MAX_CLIENT, -1);
		if (epoll_ret < 0)
		{
			perror("epoll_ret");
			throw std::logic_error("epoll_ret");
		}
		for (int i = 0; i < epoll_ret; i++)
		{
			/* if (_events[i].events == 0) */
			/* 	continue; */

			std::cout << TURQ << "IN FOR LOOP" << RESET << std::endl;
			fd = _events[i].data.fd;
			assert(recv(fd, buf, BUFFSIZE, 0) >= 0);
			header[fd].setRawData(buf);
			status = header[fd].parseRequest();
			header[fd].printInfo();
			header[fd].sendResponse(fd);
			header[fd].clear();
			std::cout << BLUE << "status is " << Header::getReasonPhrase(status) << RESET << std::endl;
			bzero(buf, BUFFSIZE);
			close(fd);
			_client--;
		}
		epoll_ret = 0;

	}
	close(server_sock.getSocketFd());
	std::cerr << "end;" << std::endl;
	
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

