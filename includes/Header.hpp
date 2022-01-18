#ifndef HEADER
# define HEADER

#include "webserv.hpp"

class Header
{
private:
	int		_row;
	int		_ret;
	int		_fd;
	char	*_buff;


	std::string _method;
	std::string	_version;
	std::string	_URI;
	std::string _query;
	std::string	_fileToSend;
	std::map<std::string, std::string> _headerField;
	std::map<std::string, std::string> _errorCode;
	
public:
	std::map<std::string , std::string>	getRequest(void);
	std::string							getMethod(void);
	std::string					getURI(void);
	std::string					getReasonPhrase(std::string);
	std::string					getReasonPhrase(int);
	std::string					getErrorPage(int code);
	void						setRawData(char *);
	void						setFd(int);
	int							getFd(void);

	void						initErrorCode(void);
	int							checkURI(void);
	int							isFile(std::string);
	int							isDir(std::string);
	void						OpenResponsFile(const char *path);

	int							parseStartLine(std::string);
	int							parseHeaderfield(std::string);
	void						printHeaderInfo(void);
	int							parseRequest(void);
	
	int							sendRespons(int fd);
	int							sendHeader(int fd);
	void						clear(void);
	Header();
	Header(char *);
	~Header();
};

#endif