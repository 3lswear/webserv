#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#define RED			"\033[31m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define YELLO		"\033[33m"
#define BLUE		"\033[34m"
#define PINK		"\033[35m"
#define TURGUOISE   "\033[36m"
#define TURQ   "\033[36m"
#define ZERO_C		"\033[0m"
#define RESET		"\033[0m"

#define	HOME		"www"
#define BUFFSIZE	65536
#define MAX_CLIENT	1000
#define WHITESPACE " \n\r\t\f\v"



#include <iostream>
#include <string>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <wait.h>
#include <exception>
#include <csignal>

#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <dirent.h>


#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <algorithm>
#include <cctype>


#ifdef DEBUG
#define DBOUT std::cerr << TURQ
#define ENDL RESET << std::endl
#else
#define DBOUT 0 && std::cerr
#define ENDL RESET << std::endl
#endif

#endif
