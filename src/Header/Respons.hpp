#ifndef RESPONS_HPP
#define RESPONS_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Autoindex.hpp"

class Respons
{
private:
    std::string     _body;
    std::string     _header;
    Request         _request;
    ServerConfig    *_config;

private:
    std::map<std::string, std::string> _errorCode;
    
public:
    std::string getHeader(void);
    std::string getBody(void);
    std::string					getReasonPhrase(std::string);
	std::string					getReasonPhrase(int);
	std::string					getErrorPage(int code);


    void        setData(Request, ServerConfig *);
public:

	void						OpenResponsFile(const char *path);
	void						initErrorCode(void);
    void                        generate();
    Respons();
    ~Respons();
};



#endif