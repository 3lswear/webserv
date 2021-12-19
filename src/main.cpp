#include <iostream>
#include "webserv.hpp"
#include <exception>

#include <cstdio>

#include <fstream>

#define PORT 8080
#define BUFSIZE 2048

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int socketfd;
	int opt = 1;
	int new_socket;

	char buffer[BUFSIZE + 1] = {0};
	char sendbuffer[BUFSIZE + 1] = {0};

	struct sockaddr_in address;

	std::cout << "jopa" << std::endl;

	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		std::cerr << "socket bad" << std::endl;
		return (1);
	}
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "setsockopt" << std::endl;
		return (1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(socketfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "bind" << std::endl;
		return (1);
	}

	if (listen(socketfd, 3) < 0)
	{
		std::cerr << "listen" << std::endl;
		return (1);
	}

	socklen_t addrlen = sizeof(address);
	if ((new_socket = accept(socketfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
	{
		perror("accept error");
		throw std::runtime_error("accept error");
	}

	int val_read = recv(new_socket, buffer, BUFSIZE, 0);
	std::cout << "read value: " << val_read << std::endl;
	std::cout << "buffer: \n\"" << buffer << "\"" << std::endl;

	/* HEADER */

	/* std::string protocol = "HTTP/2 200 OK\r\nserver: jopa\r\n"; */
	/* std::string type = "Content-Type: text/html\r\n"; */
	/* std::string listen */

	/* std::string tmp = protocol + type + "\r\n"; */
	std::string tmp = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: Apache/2.2.14 (Win32)\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 6196\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n";
	const char *payload = tmp.c_str();

	std::cout << "<<<" << payload << ">>>" << std::endl;
	send(new_socket, payload, tmp.length(), 0);

	std::ifstream page("www/index.html");
	if (!page.good())
	{
		throw std::runtime_error("file bad");
	}

	while (!page.eof())
	{
		page.read(sendbuffer, BUFSIZE);
		send(new_socket, sendbuffer, page.gcount(), 0);
	}

	close(new_socket);
	while(1);
	close(socketfd);

	return (0);
}
