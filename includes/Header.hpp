#ifndef HEADER
# define HEADER

#include "webserv.hpp"

class Header
{
private:
	int		_row;
	char	*_buff;


	std::string _method;
	std::string	_version;
	std::string	_URI;
	std::map<std::string, std::string> _headerField;
	

public:
	enum REQ
	{
		GET,
		POST,
		DELETE
	};

public:
	std::map<std::string, std::string>	getRequest(void);
	std::string							getMethod(void);
	std::string					getURI(void);
	void						setRawData(char *);
	int							parseStartLine(std::string);
	int							parseHeaderfield(std::string);
	void						printHeaderInfo(void);
	int							parseRequest(void);
	void						clear(void);
	Header();
	Header(char *);
	~Header();
};

#endif