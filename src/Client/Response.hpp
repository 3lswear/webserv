#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Autoindex.hpp"

class Response
{
private:
    std::string     _body;
    std::string     _Client;
    Request         _request;
    ServerConfig    *_config;

private:
    std::map<std::string, std::string> _errorCode;

private:
    void                        methodGet(void);
    // void                        methodPost(void);
    // void                        methodDelete(void);
    void                        invalidClient(void);

public:
    std::string                 getClient(void);
    std::string                 getBody(void);
    std::string					getReasonPhrase(std::string);
	std::string					getReasonPhrase(int);
	std::string					getErrorPage(int code);


    void                        setData(Request, ServerConfig *);
public:
	void						OpenResponseFile(const char *path);
	void						initErrorCode(void);
    void                        generate();
    Response();
    ~Response();

};


#endif