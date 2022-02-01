#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "ServerConfig.hpp"
#include "webserv.hpp"


class Request
{
private:
	char		*_data;
	
	int			_ret;
	int			_row;
	int			_lifeTime;
	unsigned int			_contentLength;
	unsigned int			_received;		
	unsigned int 			_headerSize;

	std::string _URI;
	std::string	_head;
	std::string	_body;
	std::string	_host;
    std::string _query;
	std::string _method;
    std::string _fullURI;
    std::string _version;
	std::string	_location;
	std::string	_connection;
	std::map<std::string, std::string> _headerField;

	ServerConfig	*_config;
	bool			_head_ok;
	bool			_body_ok;
	bool			_chunked;
public:
	std::string					getURI(void);
	std::string					getBody(void);
	std::string					getHost(void);
	std::string					getQuery(void);
	std::string					getMethod(void);
	std::string					getFullUri(void);
	std::string					getVersion(void);
	std::string					getLocation(void);
	std::string					getConnection(void);
	ServerConfig				*getConfig(void);
	int							getCode(void);
	int							getLifeTime(void);
	unsigned int				getContentLength(void) const;
	unsigned int				getHeaderSize(void) const;
	unsigned int				getRecved(void)const;
	std::map<std::string, std::string>	getClientFields(void);
	bool						getChunked(void);

	void						setConfig(ServerConfig *config);
	void						setData(char *);
	void						setData(char *, ServerConfig *);

public:

	Request();
	Request(char *str);
	int							parseStartLine(std::string);
	int							parseClientfield(std::string);
	int							parseRequest(void);
	int							parseHeader(void);
	void						parseURI(std::string);
	void						printClientInfo(void);


	bool						badCode(int);
	bool						ok(void);
	int							isDir(std::string path);
	int							isFile(std::string path);
	bool						autoindexOn(void);
	void						copyFromMap(void);
	void						clear(void);
	void						splitData(char *);

	void						increaseRecvCounter(unsigned int n);
	~Request();
};



#endif
