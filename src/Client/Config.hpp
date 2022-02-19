#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"

class Request;

class Config
{
private:
    
public:
    Config();
    ~Config();

    static  int             calcLen(std::string &, std::string &);
    static  std::vector<location *> getLocation(std::vector<location *> &, std::string &URI);
    static  ServerConfig    *getConfig(std::vector<ServerConfig *> &, Request &request, serverListen &data);
private:

};

#endif