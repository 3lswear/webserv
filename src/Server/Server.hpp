#ifndef SERVER_HPP
#define SERVER_HPP

#include "webserv.hpp"
#include "Client.hpp"
#include "Socket.hpp"

class Client;

class Server
{
	private:
		int	_port;
		int	_epoll_fd;

		TOMLMap *_root;
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
		typedef	struct s_tmp_fd
		{
			serverListen	ip_port;
			struct timeval	last_modif;
		}t_tmp_fd;
		bool	TimeToDie(struct timeval &last_modif, int lifeTime);
		int		delete_fd(std::map<int, t_tmp_fd *> &map, int fd);
		std::map<int, t_tmp_fd *>	free_socket;

	public:
		Server();

		void readConfig(char *filename);
		void setup_server_socks(std::map<int, Socket> &configurations_map);
		void run(void);
		~Server();
};


#endif
