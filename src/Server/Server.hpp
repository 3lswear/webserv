#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"
#include "Socket.hpp"
#include "parse.hpp"


class Client;

class Server
{
	private:
		int	_port;
		int	_epoll_fd;
		struct epoll_event	_events[MAX_CLIENT];
		struct sockaddr_in _addres;
		std::string _ip;
		std::vector<ServerConfig *>	_configs;
		void add_to_epoll_list(int fd, unsigned int ep_events);

		static void print_epoll_events(unsigned int events);

	private:
		void checkError(int fd, std::string str);
		void sendFile(std::string str);
		void sendClient(Client head, int);
		void sendResponse(Client head, int);
		void setNonBlock(int fd);
		void sendData(Client &client, int fd);
		void readSocket(Client &client, int fd);
		int delete_client(std::map<int,Client *> &map, int fd);

		static void clean_generic(toml_node *node);
		static void clean_parsed(TOMLMap *root);

		enum e_req_status
		{
			READING,
			WRITING,
			ENDED
		}; 

		typedef struct s_client_status
		{
			int fd;
			int serverfd;
			size_t readn;
			size_t left;
			enum e_req_status req_status;
		} t_client_status;

	public:
		Server();
		Server(std::string path);

		void readConfig(char *filename);
		void setupConfig(void);
		void start(void);
		void end(void);
		~Server();
};


#endif
