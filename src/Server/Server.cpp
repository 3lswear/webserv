#include "Server.hpp"
#include <exception>

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

	sent = send(fd, tmp + client.getCounter(), send_len, MSG_NOSIGNAL);
	DBOUT << getDebugTime()
		<< " sent "
		<< send_len <<
		"/" << sent <<
		" to client " << fd << ENDL;
	if (sent <= 0)
	{
		DBOUT << getDebugTime() << FAIL << "SEND FAILED" << ENDL;
		client.done = true;
	}
	else if (sent > 0)
		client.increaseCounter();
}

void Server::readSocket(Client &client, int fd)
{
	int bytes_read;
	std::string	stringBUF(BUFFSIZE, 0);

	DBOUT << TURQ << "IN readSocket" << ENDL;
	bytes_read = recv(fd, &stringBUF[0], BUFFSIZE, 0);
	if (bytes_read == 0)
	{
		DBOUT << getDebugTime() << FAIL << "bytes_read 0" << ENDL;
		client.done = true;
	}
	else if (bytes_read == -1)
	{
		DBOUT << getDebugTime() << FAIL << "bytes_read -1" << ENDL;
		client.done = true;
	}
	else
	{
		stringBUF.erase(bytes_read, stringBUF.size());
		client.setRawData(stringBUF);
		client.increaseRecvCounter(bytes_read);
		client.parseRequest();
		if (client.allRecved())
			client.allRead = true;
	}
}

inline int Server::delete_client_force(std::map<int, Client *> &client_map, int fd)
{
	DBOUT << WARNING << getDebugTime() << OKCYAN
		<< "force deleting client "
		<< fd
		<< ENDL;

	epoll_handle(EPOLL_CTL_DEL, fd, 0);
	close(fd);
	client_map[fd]->clear();
	delete (client_map[fd]);
	client_map.erase(fd);
	return (0);
}


inline int Server::delete_client(std::map<int, Client *> &client_map, int fd)
{
	if (client_map[fd]->getRequest().getConnection() == "close")
	{
		DBOUT << WARNING << getDebugTime()
			<< "completely deleting client "
			<< fd
			<< ENDL;

		epoll_handle(EPOLL_CTL_DEL, fd, 0);
		close(fd);
		client_map[fd]->clear();
		delete (client_map[fd]);
		client_map.erase(fd);
	}
	else
	{
		DBOUT << WARNING << getDebugTime()
			<< "deleting only client "
			<< fd
			<< ENDL;

		t_fd_info *tmp_fd;

		epoll_handle(EPOLL_CTL_MOD, fd, EPOLLIN);

		tmp_fd = new t_fd_info;
		tmp_fd->ip_port = client_map[fd]->getIpPort();
		gettimeofday(&tmp_fd->last_modif, NULL);
		vacant_fds[fd] = tmp_fd;

		client_map[fd]->clear();
		client_map[fd]->~Client();
	}
	return (0);
}

inline int		Server::delete_fd(std::map<int, t_fd_info *> &map,
		std::map<int, t_fd_info *>::iterator &it,
		std::map<int, Client *> &client_map)
{
	epoll_handle(EPOLL_CTL_DEL, it->first, 0);
	DBOUT << WARNING << getDebugTime()
		<< "deleting fd "
		<< it->first
		<< ENDL;
	delete it->second;
	close(it->first);
	client_map.erase(it->first);
	map.erase(it++);

	return (0);
}


void	Server::setup_server_socks(std::map<int, Socket> &configurations_map)
{
	unsigned int server_events = EPOLLIN;

	for (std::vector<ServerConfig *>::iterator it = _configs.begin();
			it != _configs.end(); ++it)
	{
		ServerConfig *config = *it;
		std::map<int, Socket>::iterator sock_it;

		for (sock_it = configurations_map.begin();
				sock_it != configurations_map.end(); ++sock_it)
		{
			if ((config->getPort() == sock_it->second.port) && (config->getHost() == sock_it->second.ip))
			{
				break;
			}
		}
		if (sock_it != configurations_map.end())
			continue;
		Socket server_sock(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0, config->getPort(), config->getHost());
		if (server_sock.init(MAX_CLIENT) >= 0)
		{
			/* configurations_map[server_sock.getSocketFd()] = server_sock; */
			configurations_map.insert(std::make_pair(server_sock.getFd(), server_sock));
			DBOUT << YELLO << "adding server_sock..." << ENDL;
			add_to_epoll_list(server_sock.getFd(), server_events);

			std::cerr << getDebugTime() << GREEN
				<< "listening on "
				<< config->getHost()
				<< ":"
				<< config->getPort()
				<< ENDL;
		}
		else
		{
			std::cerr << getDebugTime() << FAIL
				<< "failed to bind to "
				<< config->getHost()
				<< ":"
				<< config->getPort()
				<< ENDL;
		}
	}
	if (configurations_map.empty())
		throw std::domain_error("No servers were set up. Exiting.");
}

inline void Server::epoll_handle(int op, int fd, int event)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = event;

	if (epoll_ctl(_epoll_fd, op, fd, &ev))
	{
		std::cerr << getDebugTime() << FAIL;
		perror("epoll_ctl");
		std::cerr << RESET;
	}
}

inline void	Server::add_to_epoll_list(int fd, unsigned int ep_events)
{
	epoll_handle(EPOLL_CTL_ADD, fd, ep_events);
	setNonBlock(fd);
	DBOUT << GREEN
		<< "add socket "
		<< fd
		<< " to epoll_list"
		<< ENDL;
}

bool	Server::TimeToDie(struct timeval &last_modif, int lifeTime)
{
	struct timeval curTime;

	gettimeofday(&curTime, NULL);
	if ((curTime.tv_sec - last_modif.tv_sec) >= lifeTime)
		return (true);
	else
		return (false);
}

void	sigHandler(int sig)
{
	if (sig == SIGINT)
	{
		std::cerr << "\n";
		std::cerr << getDebugTime() << FAIL << "SIGINT called. Server shutdown!" << std::endl;
		exit(-1);
	}
}

void	Server::run(void)
{
	std::map<int, Client*> client_map;
	std::map<int, Socket> configurations_map;

	std::map<int, t_fd_info *>::iterator fd_it;

	unsigned int client_events = EPOLLIN;

	std::signal(SIGINT, sigHandler);

	_epoll_fd = epoll_create(1337);
	setNonBlock(_epoll_fd);

	setup_server_socks(configurations_map);

	while (1)
	{
		int ready_num = epoll_wait(_epoll_fd, _events, MAX_CLIENT, 5000);

		if (ready_num < 0)
		{
			std::cerr << getDebugTime() << FAIL;
			perror("epoll_wait");
			std::cerr << RESET;
		}
		
		for (int i = 0; i < ready_num; i++)
		{
			int fd = _events[i].data.fd;
			unsigned int events = _events[i].events;
			std::map<int, Socket>::iterator sock_it;

			DBOUT << "FD is " << fd << ENDL;
			print_epoll_events(events);

			if ((events & EPOLLIN)
					&& (sock_it = configurations_map.find(fd)) != configurations_map.end())
			{
				int client_sock = accept(fd,
						(sock_it->second).getAddr(), (sock_it->second).getLen());
				if (client_sock > 0)
				{
					client_map[client_sock] = new Client((sock_it->second).min_config);
					add_to_epoll_list(client_sock, client_events);
				}
				else
				{
					std::cerr << getDebugTime() << WARNING;
					perror("Accept");
					std::cerr << RESET;
				}
			}
			else
			{
				if (events & EPOLLIN)
				{
					fd_it = vacant_fds.find(fd);
					if (fd_it != vacant_fds.end())
					{
						DBOUT << "creating new client for vacant fd" << ENDL;
						new (client_map[fd]) Client(fd_it->second->ip_port);
						delete fd_it->second;
						vacant_fds.erase(fd);
					}
					readSocket(*client_map[fd], fd);
					if (client_map[fd]->done)
						delete_client_force(client_map, fd);
					else if (client_map[fd]->readyToSend())
					{
						client_map[fd]->generateRespons(_configs);

						epoll_handle(EPOLL_CTL_MOD, fd, EPOLLOUT);
						DBOUT << getDebugTime() << "rearmed to EPOLLOUT" << ENDL;
					}
				}
				else if (events & EPOLLOUT)
				{
					sendData(*client_map[fd], fd);
					if (client_map[fd]->allSended())
					{
						delete_client(client_map, fd);
					}
				}
			}
		}
		fd_it = vacant_fds.begin();
		while (fd_it != vacant_fds.end())
		{
			if (TimeToDie(fd_it->second->last_modif, LIFE_TIME))
			{
				free(client_map[fd_it->first]);
				delete_fd(vacant_fds, fd_it, client_map);
			}
			else
				++fd_it;
		}
	}
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
		// (*pri)->printFields();
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

