#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define RED			"\033[31m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define BLUE		"\033[34m"
#define PINK		"\033[35m"
#define TURGUOISE   "\033[36m"
#define ZERO_C		"\033[0m"
#define	HOME		"www"


#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <map>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <cctype>
#include "Socket.hpp"
#include "Autoindex.hpp"
#include "Server.hpp"
#include "Header.hpp"

#endif
