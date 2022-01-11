#ifndef HEADER
# define HEADER

#include "webserv.hpp"

class Header
{
private:
	int		_type;
	int		_row;
	char	*_buff;
	std::string _fileName;
	std::map<std::string, std::string> _request;

public:
	enum REQ
	{
		GET,
		POST,
		DELETE
	};

public:
	std::map<std::string, std::string>	getRequest(void);
	int							getType(void);
	std::string					getFileName(void);
	void						setFile(std::string);
	void						setRequest(char *);
	void						identifyType(std::string);
	void						printHeaderInfo(void);
	void						parseRequest(void);
	void						clearHeader(void);
	Header();
	Header(char *);
	~Header();
};

#endif