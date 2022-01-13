#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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
#include "Server.hpp"
#include <stdlib.h>
#include <fstream>

#include <map>
#include <vector>

void parse(void);

class toml_node;

/* typedef std::vector<TOMLMap *>	TOMLArray; */
/* typedef std::vector<TOMLArray *> TOMLArrayOfMap; */
typedef std::map<std::string, toml_node *> TOMLMap; // = JSONObject
typedef std::pair<std::string, std::vector<TOMLMap *> *> TOMLMapArray;
typedef std::vector<toml_node *> TOMLArray;

void display(TOMLMap *config);

#endif
