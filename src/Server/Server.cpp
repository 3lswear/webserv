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
	bzero(_events, sizeof(_events));
	_client = 0;

}

Server::Server(std::string path)
{
	(void)path;
	_client = 0;
}

void Server::print_epoll_events(unsigned int events)
{
	DBOUT << "Epoll events: ";
	if (events & EPOLLIN)
		DBOUT << "EPOLLIN ";
	if (events & EPOLLOUT)
		DBOUT << "EPOLLOUT ";
	if (events & EPOLLET)
		DBOUT << "EPOLLET ";
	DBOUT << ENDL;
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

void Server::sendData(Client &client, int fd)
{
	std::string tmp = client.getStrToSend();
	unsigned int size_diff = tmp.size() - client.getCounter();


	if (size_diff < BUFFSIZE)
	{
		tmp = tmp.substr(client.getCounter(), size_diff);
	}
	else
		tmp = tmp.substr(client.getCounter(), BUFFSIZE);

	/* std::cout << YELLO << tmp << RESET << std::endl; */
	/* std::cout << GREEN << client.getCounter() << RESET << std::endl; */


	send(fd, tmp.c_str(), tmp.size(), 0);
	client.increaseCounter();

}

void Server::readSocket(Client &client, int fd)
{

	int status;
	int bytes_read;
	char buf[BUFFSIZE + 1] = {0};

	std::cout << TURQ << "IN readSocket" << RESET << std::endl;
	bytes_read = recv(fd, buf, BUFFSIZE, 0);
	if (bytes_read == 0)
	{
		client.allRead = true;
		return;
	}
	client.setRawData(buf);
	client.increaseRecvCounter(bytes_read);
	status = client.parseRequest();
	// client_map[fd].printClientInfo();

	if ((bytes_read < BUFFSIZE) && client.allRecved())
	{
		client.allRead = true;
	}

	std::cerr << "recvCounter " << client.getRecvCounter() << std::endl;
	std::cerr << "contentLength " << client.getRequest().getContentLength() << std::endl;
	std::cerr << "allRead " << client.allRead << std::endl;

	std::cout << BLUE << "status is " << Response::getReasonPhrase(status) << RESET << std::endl;
	bzero(buf, BUFFSIZE);
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
	std::cerr << YELLO
		<< "add socket "
		<< fd
		<< " to epoll_list"
		<< RESET << std::endl;
}

void	Server::start(void)
{
	Socket server_sock(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1");
	std::map<int, Client> client_map;
	std::map<int, Client>::iterator client_it;
	int fd;
	int ready_num = 0;

	unsigned int client_events = EPOLLIN | EPOLLOUT | EPOLLET;
	unsigned int server_events = EPOLLIN | EPOLLOUT | EPOLLET;
	
	_epoll_fd = epoll_create(1337);
	checkError(server_sock.init(MAX_CLIENT), "Socket init");
	setNonBlock(server_sock.getSocketFd());
	setNonBlock(_epoll_fd);

	std::cerr << YELLO << "adding server_sock..." << RESET << std::endl;
	add_to_epoll_list(server_sock.getSocketFd(), server_events);
	while (1)
	{

		// sending
		for (client_it = client_map.begin();
				client_it != client_map.end(); ++client_it)
		{
			std::cout << TURQ << "IN SEND LOOP" << RESET << std::endl;
			Client &client = client_it->second;

			if (!client.allRead && !client.isEmpty())
			{
				readSocket(client, fd);
			}
			if (client.readyToSend())
			{
				epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, client_it->first, NULL);
				std::cout << TURQ << "readyToSend " << client_it->first << RESET << std::endl;
				client.generateRespons();

				sendData(client, client_it->first);
			
			}

			if ((client.readyToSend() && client.allSended()) || client.isEmpty())
			{
				/* SUS */
				/* client_map[fd].printClientInfo(); */
				close(client_it->first);
				std::cerr << RED <<
					"deleting client "
					<< client_it->first
					<<
					RESET
					<< std::endl;
				client_map.erase(client_it);
			}
		}

		ready_num = epoll_wait(_epoll_fd, _events, MAX_CLIENT, 100);
		/* std::cout << TURQ << "ready_num " << ready_num << RESET << std::endl; */

		if (ready_num < 0)
			throw std::logic_error("epoll_ret");
		for (int i = 0; i < ready_num; i++)
		{
			fd = _events[i].data.fd;

			DBOUT << "FD is " << fd << ENDL;
			print_epoll_events(_events[i].events);

			if (fd == server_sock.getSocketFd())
			{
				int client_sock = accept(server_sock.getSocketFd(),
						server_sock.getSockaddr(), server_sock.getSocklen());
				if (client_sock > 0)
					add_to_epoll_list(client_sock, client_events);
				else
					throw std::logic_error("accept didnt work");
			}
			else
			{
				std::cout << TURQ << "IN FOR LOOP" << RESET << std::endl;
				/* _client--; */
				client_map[fd];
				readSocket(client_map[fd], fd);
			}
		}
		ready_num = 0;

	}
	close(server_sock.getSocketFd());
	std::cerr << "end;" << std::endl;
}



void	Server::end(void)
{
	std::vector<ServerConfig *>::iterator pri;
	std::vector<location *>::iterator	loc;
	std::vector< location *> l;

	pri = _configs.begin();
	while (pri != _configs.end())
	{
		(*pri)->printFields();
		l = (*pri)->getLocations();
		loc = l.begin();
		while (loc != l.end())
		{
			delete *loc;
			loc++;
		}
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

