#ifndef WEBSERV_HPP
#define WEBSERV_HPP

//colors
#define HEADER  "\033[95m"
#define PINK    "\033[35m"

#define BLUE	"\033[34m"
#define OKBLUE  "\033[94m"

#define FAIL    "\033[91m"
#define RED		"\033[31m"

#define GREEN	"\033[32m"
#define OKGREEN "\033[92m"

#define YELLOW	"\033[33m"
#define YELLO	"\033[33m"
#define WARNING "\033[93m"

#define TURGUOISE   "\033[36m"
#define OKCYAN  "\033[96m"

#define TURQ	"\033[36m"

#define BOLD	"\033[1m"

#define ENDC	"\033[0m"
#define ZERO_C  "\033[0m"
#define RESET   "\033[0m"

#define	HOME		"www"
#define BUFFSIZE	65536
#define LIFE_TIME   5
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
#include <ctime>
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
