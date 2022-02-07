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

}

Server::Server(std::string path)
{
	(void)path;
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

	clean_parsed(root);
}

void Server::sendData(Client &client, int fd)
{
	/* std::string tmp = client.getStrToSend(); */
	char *tmp = client.getStrToSend();
	size_t size_diff = client.response_len - client.getCounter();
	size_t send_len;

	if (size_diff < BUFFSIZE)
		send_len = size_diff;
	else
		send_len = BUFFSIZE;

	/* DBOUT << YELLO << tmp << ENDL; */
	/* DBOUT << GREEN << client.getCounter() << ENDL; */


	if (send(fd, tmp + client.getCounter(), send_len, MSG_NOSIGNAL) < 0)
	{
		DBOUT << RED << "SEND FAILED !@!!!" << ENDL;
		client.done = true;
	}
	client.increaseCounter();

}

void Server::readSocket(Client &client, int fd)
{

	int status;
	int bytes_read;
	// char buf[BUFFSIZE + 1];
	std::string	stringBUF(BUFFSIZE, 0);

	DBOUT << TURQ << "IN readSocket" << ENDL;
	DBOUT << "client in readSocket "<< &client << ENDL;
	bytes_read = recv(fd, &stringBUF[0], BUFFSIZE, 0);
	if (bytes_read == 0)
	{
		client.allRead = true;
		return;
	}
	// buf[bytes_read + 1] = '\0';
	stringBUF.erase(bytes_read, stringBUF.size());
	client.setRawData(stringBUF);
	// client.setRawData(buf);
	client.increaseRecvCounter(bytes_read);
	status = client.parseRequest();
	// client_map[fd].printClientInfo();

	if ((bytes_read < BUFFSIZE) && client.allRecved())
	{
		client.allRead = true;
	}

	DBOUT << GREEN << "recvCounter " << client.getRecvCounter() << ENDL;
	DBOUT << GREEN << "contentLength " << client.getRequest().getContentLength() << ENDL;
	DBOUT << GREEN << "allRead " << client.allRead << ENDL;

	DBOUT << BLUE << "status is " << Response::getReasonPhrase(status) << ENDL;
}

int Server::delete_client(std::map<int,Client *> &client_map, int fd)
{
	int ret;
	ret = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	client_map[fd]->clear();
	delete (client_map[fd]);
	client_map.erase(fd);
	DBOUT << RED <<
		"deleting client "
		<< fd
		<< ENDL;
	return (ret);
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
	DBOUT << YELLO
		<< "add socket "
		<< fd
		<< " to epoll_list"
		<< ENDL;
}

void	Server::start(void)
{
	Socket server_sock(AF_INET, SOCK_STREAM, 0, _port, "127.0.0.1");
	std::map<int, Client*> client_map;
	int fd;
	int ready_num = 0;

	unsigned int client_events = EPOLLIN;
	unsigned int server_events = EPOLLIN;
	
	_epoll_fd = epoll_create(1337);
	checkError(server_sock.init(MAX_CLIENT), "Socket init");
	setNonBlock(server_sock.getSocketFd());
	setNonBlock(_epoll_fd);

	DBOUT << YELLO << "adding server_sock..." << ENDL;
	add_to_epoll_list(server_sock.getSocketFd(), server_events);
	while (1)
	{

		ready_num = epoll_wait(_epoll_fd, _events, MAX_CLIENT, -1);
		/* DBOUT << TURQ << "ready_num " << ready_num << ENDL; */

		if (ready_num < 0)
			throw std::logic_error("epoll_ret");
		for (int i = 0; i < ready_num; i++)
		{
			fd = _events[i].data.fd;
			unsigned int events = _events[i].events;

			/* DBOUT << "FD is " << fd << ENDL; */
			/* print_epoll_events(events); */

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
				if (client_map.find(fd) == client_map.end())
					client_map[fd] = new Client();
				if (events & EPOLLIN)
				{
					readSocket(*client_map[fd], fd);
					if (client_map[fd]->readyToSend())
					{
						client_map[fd]->generateRespons();

						struct epoll_event ev;

						ev.events = EPOLLOUT;
						ev.data.fd = fd;
						assert( epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == 0);
						DBOUT << GREEN << "rearmed to EPOLLOUT" << ENDL;
					}
					if (client_map[fd]->isEmpty())
						delete_client(client_map, fd);
				}
				else if (events & EPOLLOUT)
				{
					/* DBOUT << GREEN << "doing sendData" << ENDL; */
					/* client_map[fd].printClientInfo(); */
					sendData(*client_map[fd], fd);
					if (client_map[fd]->allSended())
					{
						delete_client(client_map, fd);
					}
				}
			}
		}

	}
	close(server_sock.getSocketFd());
	DBOUT << RED << "end;" << ENDL;
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
		DBOUT << RED << "Server ERROR: " << str << ENDL;
		exit(1);
	}
	else
		DBOUT << GREEN << "Server SUCCESS: " << str << ENDL;
}

void Server::clean_generic(toml_node *node)
{
	switch (node->type)
	{
		case toml_node::STRING:
		{
			DBOUT << "cleaning string" << ENDL;
			delete node->getString();
		}
		break;
		case toml_node::MAPARRAY:
		{
			DBOUT << "cleaning MAPARRAY" << ENDL;
			TOMLMapArray *map_array = node->getMapArray();
			for (TOMLMapArray::iterator it = map_array->begin();
					it != map_array->end(); ++it)
			{
				DBOUT << "cleaning a MAP of MAPARRAY" << ENDL;
				TOMLMap *map = *it;
				TOMLMap::iterator map_it = map->begin();
				for (map_it = map->begin();
						map_it != map->end(); ++map_it)
				{
					DBOUT << "cleaning a MAP item " << map_it->first << ENDL;
					clean_generic(map_it->second);
					/* map->erase(map_it); */
				}
				map->clear();
				delete map;
			}
			map_array->clear();
			delete map_array;
			DBOUT << "end cleaning MAPARRAY" << ENDL;
		}
		break;
		case toml_node::MAP:
		{
			DBOUT << "cleaning MAP" << ENDL;
			TOMLMap *map = node->getMap();
			for (TOMLMap::iterator it = map->begin(); it != map->end(); ++it)
			{
				DBOUT << "key is " << it->first << ENDL;
				clean_generic(it->second);
				/* map->erase(it); */
			}
			map->clear();
			delete map;
		}
		break;

		case toml_node::ARRAY:
		{
			DBOUT << "cleaning ARRAY" << ENDL;
			TOMLArray *arr = node->getArray();
			for (TOMLArray::iterator it = arr->begin();
					it != arr->end(); ++it)
			{
				clean_generic(*it);
			}
			arr->clear();
			delete arr;
			DBOUT << "end cleaning MAP" << ENDL;
		}
		break;

		default:
		{
			DBOUT << "Cleaning type " << node->type << " not implemented :)" << ENDL;
		}
	}
	delete node;

}

void Server::clean_parsed(TOMLMap *root)
{
	TOMLMap::iterator it;

	DBOUT << ">>> cleaning up: <<<" << std::endl;
	for (it = root->begin(); it != root->end(); ++it)
	{
		/* DBOUT << RED << it->first */
		/* 	<< ": " << GREEN */
		/* 	<< *(it->second->toString()); */

		clean_generic(it->second);
		/* delete it->second; */
		std::cout << ", " << std::endl;
	}
	DBOUT << YELLO << "end of clean" << ENDL;
	root->clear();
	delete root;
	root = NULL;
}

Server::~Server()
{
	
}

