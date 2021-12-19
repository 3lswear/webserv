#include "webserv.hpp"
//----------------------------------------------Constructors-----------------------------------------------------------------------------------
Server::Server()
{

}

Server::Server(std::string path)
{
	(void)path;
}
//---------------------------------------------Color printing----------------------------------------------------------------------------------

void	Server::printRed(std::string str)
{
	std::cout << "\033[31m" << str << "\033[0m";
}

void	Server::printGreen(std::string str)
{
	std::cout << "\033[32m" << str << "\033[0m";
}

void	Server::printYellow(std::string str)
{
	std::cout << "\033[33m" << str << "\033[0m";
}

void	Server::printBlue(std::string str)
{
	std::cout << "\033[34m" << str << "\033[0m";
}

void	Server::printPink(std::string str)
{
	std::cout << "\033[35m" << str << "\033[0m";
}

void	Server::printTurguoise(std::string str)
{
	std::cout << "\033[36m" << str << "\033[0m";
}

//---------------------------------------------Send--------------------------------------------------------------------------------------------

void	Server::sendHeader(void)
{
	std::string	tmp;
	const char	*header;

	tmp = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 6196\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	header = tmp.c_str();
	printTurguoise("Send Header\n");
	printYellow(tmp);
	send(_clientSocket, header, tmp.length(), 0);
}

void	Server::sendFile(std::string str)
{
	char const *path = str.c_str();
	std::ifstream file(path);
	char	buff[BUFFSIZE + 1] = {0};

	if (!file.good())
	{
		printRed("Send ERROR: bad file: ");
		printRed(str);
		std::cout << std::endl;
	}
	sendHeader();
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
		printRed("Server ERROR: ");
		printRed(str);
		std::cout << std::endl;
		exit(1);
	}
	else
	{
		printGreen("Server SUCCESS: ");
		printGreen(str);
		std::cout << std::endl;
	}
}

void	Server::start(void)
{
	char	buff[BUFFSIZE + 1] = {0};
	int		opt	= 1;
	socklen_t	addrlen;

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
	printTurguoise("Receive Header\n");
	printYellow(buff);
	sendFile("www/index.html");
	close(_clientSocket);
	close(_serverSocket);
}

void	Server::end(void)
{

}

Server::~Server()
{
}

