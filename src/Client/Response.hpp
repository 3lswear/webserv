#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Autoindex.hpp"

class Response
{
private:
    std::string     _body;
    std::string     _header;
    Request         _request;
    ServerConfig    *_config;
    int             _code;

private:
    static std::map<std::string, std::string> _errorCode;

private:
    void                        methodGet(void);
    // void                        methodPost(void);
    // void                        methodDelete(void);
    void                        invalidClient(void);
    void                        generateHeader(void);
    void                        generateBody(void);
    std::string                 getTime(void);
    std::string                 getContentType(void);
public:
    std::string                 getHeader(void);
    std::string                 getBody(void);
    static std::string			getReasonPhrase(std::string);
	static std::string			getReasonPhrase(int);
	std::string					getErrorPage(int code);


    void                        setData(Request, ServerConfig *);
public:
	void						OpenResponseFile(const char *path);
	void					    initErrorCode(void);
    void                        generate();
    Response();
    ~Response();

};


#endif