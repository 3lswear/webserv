#include "Server.hpp"
#include <exception>
#include <cassert>

#define THREAD_NUM 100
#define MAX_EVENTS

Server::Server()
{
	bzero(_events, sizeof(_events));
}

void	Server::readConfig(char *filename)
{
	config::TOMLParser parser(filename);
	_root = parser.root;
	parser.parse();
	DBOUT << RED << "PARSED !!!" << ENDL;

	TOMLMap::iterator it1;
	TOMLMapArray	*arr;
	TOMLMapArray::iterator	it;


	arr = parser.root->find("server")->second->getMapArray();
	it = arr->begin();
	ServerConfig	*tmp;

	while (it != arr->end())
	{
		tmp = new ServerConfig(*it);
		_configs.push_back(tmp);
		tmp->fillFields();
		++it;
	}

	DBOUT << RED << "done processing parsed data" << ENDL;
}

void Server::sendData(Client &client, int fd)
{
	char *tmp = client.getStrToSend();
	size_t size_diff = client.response_len - client.getCounter();
	size_t send_len;
	ssize_t sent;

	if (size_diff < BUFFSIZE)
		send_len = size_diff;
	else
		send_len = BUFFSIZE;

	/* DBOUT << YELLO << tmp << ENDL; */
	/* DBOUT << GREEN << client.getCounter() << ENDL; */
	DBOUT << "sent " << send_len << " to client " << fd << ENDL;
	sent = send(fd, tmp + client.getCounter(), send_len, MSG_NOSIGNAL);
	if (sent < 0)
	{
		DBOUT << RED << "SEND FAILED !@!!!" << ENDL;
		client.done = true;
	}
	else if (sent > 0)
		client.increaseCounter();

}

void Server::readSocket(Client &client, int fd)
{
	int status;
	int bytes_read;
	std::string	stringBUF(BUFFSIZE, 0);

	DBOUT << TURQ << "IN readSocket" << ENDL;
	// DBOUT << "client in readSocket "<< &client << ENDL;
	bytes_read = recv(fd, &stringBUF[0], BUFFSIZE, 0);
	if (bytes_read == 0)
	{
		DBOUT << RED << "bytes_read 0" << ENDL;
		client.done = true;
	}
	else if (bytes_read == -1)
	{
		DBOUT << RED << "bytes_read -1" << ENDL;
		client.allRead = true;
	}
	else
	{
		stringBUF.erase(bytes_read, stringBUF.size());
		client.setRawData(stringBUF);
		client.increaseRecvCounter(bytes_read);
		status = client.parseRequest();
		// client.printClientInfo();
		if (client.allRecved())
			client.allRead = true;

		DBOUT << GREEN << "recvCounter " << client.getRecvCounter() << ENDL;
		DBOUT << GREEN << "contentLength " << client.getRequest().getContentLength() << ENDL;
		DBOUT << GREEN << "allRead " << client.allRead << ENDL;

		DBOUT << BLUE << "status is " << Response::getReasonPhrase(status) << ENDL;
	}
}

int Server::delete_client(std::map<int, Client *> &client_map, int fd)
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

void	Server::setup_server_socks(std::map<int, Socket> &configurations_map)
{
	unsigned int server_events = EPOLLIN;

	for (std::vector<ServerConfig *>::iterator it = _configs.begin();
			it != _configs.end(); ++it)
	{
		ServerConfig *config = *it;
		Socket server_sock(AF_INET, SOCK_STREAM, 0, config->getPort(), config->getHost());
		if (server_sock.init(MAX_CLIENT) >= 0)
		{
			setNonBlock(server_sock.getSocketFd());
			/* configurations_map[server_sock.getSocketFd()] = server_sock; */
			configurations_map.insert(std::make_pair(server_sock.getSocketFd(), server_sock));
			DBOUT << YELLO << "adding server_sock..." << ENDL;
			add_to_epoll_list(server_sock.getSocketFd(), server_events);

			std::cerr << GREEN
				<< config->getServerName()
				<< " started on "
				<< config->getHost()
				<< ":"
				<< config->getPort()
				<< ENDL;
		}
		else
		{
			std::cerr << RED
				<< config->getServerName()
				<< " failed to bind to "
				<< config->getHost()
				<< ":"
				<< config->getPort()
				<< ENDL;
		}
	}
	if (configurations_map.empty())
		throw std::domain_error("No servers were set up. Exiting.");
}

void	Server::add_to_epoll_list(int fd, unsigned int ep_events)
{
	struct epoll_event	ev;
	ev.events = ep_events;
	ev.data.fd = fd;

	assert(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == 0);
	setNonBlock(fd);
	DBOUT << GREEN
		<< "add socket "
		<< fd
		<< " to epoll_list"
		<< ENDL;
}

void	sigHandler(int sig)
{
	if (sig == SIGINT)
		throw ConfigException("SIGINT called. Server shutdown!");
}

void	Server::run(void)
{
	std::map<int, Client*> client_map;
	std::map<int, Socket> configurations_map;

	unsigned int client_events = EPOLLIN;

	std::signal(SIGINT, sigHandler);

	_epoll_fd = epoll_create(1337);
	setNonBlock(_epoll_fd);

	setup_server_socks(configurations_map);

	while (1)
	{

		int ready_num = epoll_wait(_epoll_fd, _events, MAX_CLIENT, 5000);
		// DBOUT << TURQ << "ready_num " << ready_num << ENDL;

		if (ready_num < 0)
			throw std::logic_error("epoll_ret");
		for (int i = 0; i < ready_num; i++)
		{
			int fd = _events[i].data.fd;
			unsigned int events = _events[i].events;
			std::map<int, Socket>::iterator sock_it;

			/* DBOUT << "FD is " << fd << ENDL; */
			/* print_epoll_events(events); */

			if ((events & EPOLLIN)
					&& (sock_it = configurations_map.find(fd)) != configurations_map.end())
			{
				int client_sock = accept(fd,
						(sock_it->second).getSockaddr(), (sock_it->second).getSocklen());
				if (client_sock > 0)
				{
					client_map[client_sock] = new Client((sock_it->second).min_config);
					add_to_epoll_list(client_sock, client_events);
				}
				else
					throw std::logic_error("accept didnt work");
			}
			else
			{
				/* if (client_map.find(fd) == client_map.end()) */
				/* 	client_map[fd] = new Client(); */
				if (events & EPOLLIN)
				{
					readSocket(*client_map[fd], fd);
					if (client_map[fd]->done)
					{
						delete_client(client_map, fd);
					}
					else if (client_map[fd]->readyToSend())
					{
						client_map[fd]->generateRespons(_configs);

						struct epoll_event ev;

						ev.events = EPOLLOUT;
						ev.data.fd = fd;
						assert( epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) == 0);
						DBOUT << GREEN << "rearmed to EPOLLOUT" << ENDL;
					}
				}
				else if (events & EPOLLOUT)
				{
					/* DBOUT << GREEN << "doing sendData" << ENDL; */
					// client_map[fd]->printClientInfo();
					sendData(*client_map[fd], fd);
					if (client_map[fd]->allSended())
					{
						delete_client(client_map, fd);
					}
				}
			}
		}

	}
	DBOUT << RED << "end;" << ENDL;
}

//----------------------------------------------Other------------------------------------------------------------------------------------------------

void	Server::setNonBlock(int fd)
{
	fcntl(fd, F_SETFL, O_NONBLOCK);
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

Server::~Server()
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

	config::clean_parsed(_root);
}

