#include "webserv.hpp"
//----------------------------------------------Constructors-----------------------------------------------------------------------------------
Server::Server()
{

}

Server::Server(std::string path)
{
	(void)path;
}

//---------------------------------------------Send--------------------------------------------------------------------------------------------

void	Server::sendHeader(Header head)
{
	std::string	tmp;
	const char *header;
	tmp = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	header = tmp.c_str();
	std::cout << TURGUOISE << "Send Header\n" << YELLOW << tmp << ZERO_C;
	send(_clientSocket, header, tmp.length(), 0);
	(void)head;
}

void	Server::sendRespons(Header head)
{
	std::string str = head.getFileName();
	const char *path = str.c_str();
	std::ifstream	file(path);
	char			buff[BUFFSIZE + 1] = {0};

	if (!file.good())
	{
		file.open("www/index2.html");
	}
	sendHeader(head);
	while (!file.eof())
	{
		file.read(buff, BUFFSIZE);
		send(_clientSocket, buff, file.gcount(), 0);
	}
}

//---------------------------------------------Configuration-----------------------------------------------------------------------------------
void	Server::readConfig(void)
{

}

void	Server::setupConfig(void)
{
	this->_ip = "127.0.0.1";
	this->_port = 8080;
}

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

void	Server::start(void)
{
	char	buff[BUFFSIZE + 1] = {0};
	int		opt	= 1;
	socklen_t	addrlen;
	Header		header;

	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	checkError(_serverSocket, "Initialize Server socket");
	checkError(setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR
		| SO_REUSEPORT, &opt, sizeof(opt)), "Set socket options");
	_addres.sin_family = AF_INET;
	_addres.sin_addr.s_addr = INADDR_ANY;
	_addres.sin_port = htons(_port);
	checkError(bind(_serverSocket, (struct sockaddr *)&_addres, sizeof(_addres)), "Bind socket");
	checkError(listen(_serverSocket, 3), "Listen socket");
	addrlen = sizeof(_addres);
	_clientSocket = accept(_serverSocket, (struct sockaddr *)&_addres, (socklen_t *)&addrlen);
	checkError(_clientSocket, "Initialize Client socket");
	checkError(recv(_clientSocket, buff, BUFFSIZE, 0), "Receive msg from client");
	std::cout << TURGUOISE << "Receive Header" << ZERO_C << std::endl;
	header.setRequest(buff);
	header.parseBuff();
	header.printHeaderInfo();
	sendRespons(header);
	close(_clientSocket);
	close(_serverSocket);
}

void	Server::end(void)
{

}

Server::~Server()
{
}

