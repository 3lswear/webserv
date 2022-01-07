#ifndef HEADER
# define HEADER

#include "webserv.hpp"

class Header
{
private:
	int		_type;
	int		_row;
	char	*_buff;
	// std::string	_respons;
	std::string _fileName;
	std::vector<std::string> _request;


public:
	enum REQ
	{
		GET,
		POST,
		DELETE
	};

public:
	std::vector<std::string>	getRequest(void);
	// std::string				getRespons(void);
	int							getType(void);
	std::string					getFileName(void);
	void				setFile(std::string);
	void				setRequest(char *);
	// void	generateRespons(void);

	void	parseBuff(void);
	void	identifyType(std::string);
	void	printHeaderInfo(void);
	Header();
	Header(char *);
	~Header();
};

#endif