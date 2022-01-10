#include "webserv.hpp"
//----------------------------------------------Constructors-----------------------------------------------------------------------------------
Server::Server()
{

}

Server::Server(std::string path)
{
	(void)path;
}

//----------------------------------------------Send--------------------------------------------------------------------------------------------

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
	(void)path;
}

//----------------------------------------------Configuration-----------------------------------------------------------------------------------
void	Server::readConfig(void)
{

}

void	Server::setupConfig(void)
{
	this->_ip = "127.0.0.1";
	this->_port = 8080;
}


void	Server::start(void)
{
	Socket		serverSocket(AF_INET, SOCK_STREAM, 0, _port);
 	char		buff[BUFFSIZE + 1] = {0};
	Header		header;

	checkError(serverSocket.init(3), "Socket init");
	_clientSocket = accept(serverSocket.getSocketFd(),
		serverSocket.getSockaddr(), serverSocket.getSocklen());
	checkError(_clientSocket, "Initialize client socket");
	checkError(recv(_clientSocket, buff, BUFFSIZE, 0), "Receive msg from client");
	std::cout << TURGUOISE << "Receive Header" << ZERO_C << std::endl;
	header.setRequest(buff);
	header.parseRequest();
	header.printHeaderInfo();
	sendRespons(header);
	close(_clientSocket);
	close(serverSocket.getSocketFd());

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

