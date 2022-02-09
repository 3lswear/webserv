#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "webserv.hpp"
#include "Request.hpp"
#include "Autoindex.hpp"

class Response
{
private:
    serverListen    _listen;
    std::string     _body;
    std::string     _header;
    Request         _request;
    ServerConfig    *_config;
    location        *_location;
    int             _code;
private:
    std::map<int, std::string>  _errorPages;
    bool                        _Autoindex;
    serverListen                _hostPort;
    std::string                 _fullURI;
    std::string                 _method;

private:
    std::string     _contentType;
    unsigned int    _contentLength;
    std::string     _server;
    std::string     _keepAlive;
    std::string     _date;
    std::string     _cacheControl;
    std::string     _locationSTR;
    std::string     _contentLanguage;
    std::string     _transferEncoding;

    void            setHeaderBlocks(void);
    void            setContentType(void);
    void            setContentLength(void);
    void            setServer(void);
    void            setConnection(void);
    void            setDate(void);
    void            setCacheControl(void);
    void            setLocation(void);
    void            setLanguage(void);
    void            setTransferEncoding(void);
private:
    static std::map<std::string, std::string> _errorCode;

private:
    void                        methodGet(void);
    void                        methodPost(void);
    void                        methodPut(void);
    void                        methodDelete(void);
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
    std::string                 getFullURI();

    bool                        isRedirect(void);
    bool                        allowedMethod(std::string &);
    void                        setData(Request, ServerConfig *);
    void                        setData(Request &, ServerConfig *, location *location);
public:
	void						OpenResponseFile(const char *path);
	void						OpenErrorFile(int code);
	void					    initErrorCode(void);
    void                        generate();
    void                        generate2(serverListen &);
    Response();
    ~Response();

};


#endif