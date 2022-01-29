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

	while (it != arr->end())
	{
		_configs.push_back(new ServerConfig(*it));
		++it;
	}
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

void	Server::add_to_epoll_list(int fd, unsigned int ep_events)
{
	struct epoll_event	ev;
	ev.events = ep_events;
	ev.data.fd = fd;

	assert(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == 0);
	_client++;
	std::cerr << RED
		<< "add client_sock "
		<< fd
		<< " to epoll_list"
		<< RESET << std::endl;
}

void	Server::start(void)
{
	Socket server_sock(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1");
	char buf[BUFFSIZE + 1] = {0};
	std::map<int, Client> Client_map;
	int fd;
	int client_sock;
	int status;
	int ready_num = 0;

	unsigned int client_events = EPOLLIN | EPOLLOUT | EPOLLET;
	
	_epoll_fd = epoll_create1(0);
	checkError(server_sock.init(MAX_CLIENT), "Socket init");
	setNonBlock(server_sock.getSocketFd());
	setNonBlock(_epoll_fd);
	while (1)
	{
		client_sock = accept(server_sock.getSocketFd(),
				server_sock.getSockaddr(), server_sock.getSocklen());
		if (client_sock > 0)
			add_to_epoll_list(client_sock, client_events);
		if (_client > 0)
			ready_num = epoll_wait(_epoll_fd, _events, MAX_CLIENT, -1);
		/* std::cout << GREEN << "after epoll_wait" << RESET << std::endl; */
		if (ready_num < 0)
		{
			perror("epoll_ret");
			throw std::logic_error("epoll_ret");
		}
		for (int i = 0; i < ready_num; i++)
		{
			/* if (_events[i].events == 0) */
			/* 	continue; */
			fd = _events[i].data.fd;
			std::cout << TURQ << "IN FOR LOOP" << RESET << std::endl;
			assert(recv(fd, buf, BUFFSIZE, 0) >= 0);
			Client_map[fd].setRawData(buf);
			status = Client_map[fd].parseRequest();
			Client_map[fd].printClientInfo();
			Client_map[fd].sendResponse(fd);
			Client_map[fd].clear();
			std::cout << BLUE << "status is " << Response::getReasonPhrase(status) << RESET << std::endl;
			bzero(buf, BUFFSIZE);
			close(fd);
			_client--;
		}
		ready_num = 0;

	}
	close(server_sock.getSocketFd());
	std::cerr << "end;" << std::endl;
}



void	Server::end(void)
{
	std::vector<ServerConfig *>::iterator pri;

	pri = _configs.begin();
	while (pri != _configs.end())
	{
		(*pri)->printFields();
		delete *pri;
		pri++;
	}
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

