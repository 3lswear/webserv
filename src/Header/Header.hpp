#ifndef HEADER_HPP
# define HEADER_HPP

#include "webserv.hpp"
#include "HeaderHandl.hpp"
#include "Autoindex.hpp"

class Header
{
private:
	HeaderHandl	_request;
	HeaderHandl	_respons;

private:
	int		_row;
	int		_ret;
	int		_autoIndex;
	int		_fd;
	char	*_buff;


	std::string	_fileToSend;
	std::map<std::string, std::string> _headerField;
	std::map<std::string, std::string> _errorCode;
	
public:
	std::map<std::string , std::string>	getHeaderField(void);
	HeaderHandl					getRequest(void);
	HeaderHandl					getRespons(void);
	std::string					getReasonPhrase(std::string);
	std::string					getReasonPhrase(int);
	std::string					getErrorPage(int code);
	void						setRawData(char *);
	void						setFd(int);
	int							getFd(void);

	void						initErrorCode(void);
	int							isFile(std::string);
	int							isDir(std::string);
	void						OpenResponsFile(const char *path);

	int							parseStartLine(std::string);
	void						parseURI(std::string);
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
