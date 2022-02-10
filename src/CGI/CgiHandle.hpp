#ifndef CGIHANDLE_HPP
#define CGIHANDLE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "ServerConfig.hpp"
#include "Response.hpp"

class Response;

class CgiHandle
{
private:
    Request                             &_request;
    Response                            &_response;
    std::map<std::string, std::string>  _variable;
    std::string                         _scriptName;
    char                                **env;
public:
    void    printSSmap(std::map<std::string, std::string> &);
    CgiHandle(Request &request, Response &response);
    void    initEnvVariables();
    void    freeEnvVariables();
    char    **mapToCharPtr();
    std::string executeCgi();
    ~CgiHandle();
};


#endif