#include "Header.hpp"

//-------------------------------------------------Constructors---------------------------------------

Header::Header()
{
	this->_fd = -1;
}

Header::Header(char *str)
{
	this->_fd = -1;
	this->_buff = str;

}

Header::Header(char *str, ServerConfig *config)
{
	this->_fd = -1;
	this->_config = config;
	this->_buff = str;

}

//-------------------------------------------------GET/SET---------------------------------------

Request Header::getRequest(void)
{
	return (_request);
}

Respons	Header::getRespons(void)
{
	return (_respons);
}

int	Header::getFd(void)
{
	return _fd;
}

void	Header::setRawData(char *str)
{
	this->_buff = str;
}

void	Header::setFd(int fd)
{
	this->_fd = fd;
}

//-------------------------------------------------Parsing---------------------------------------

int	Header::parseRequest(void)
{
	_request.setData(_buff);
	_ret = _request.parseRequest();

	return (_ret);
}

//-------------------------------------------------FILE---------------------------------------


//-------------------------------------------------SEND---------------------------------------

int	Header::sendData(int fd, std::string data)
{
	return (send(fd, data.c_str(), data.length(), 0));
}

int	Header::sendRespons(int	fd)
{
	_respons.setData(_request, _config);
	_respons.generate();
	_headerToSend = _respons.getHeader();
	_bodyToSend = _respons.getBody();
	_ret = sendData(fd, _headerToSend);
	_ret = sendData(fd, _bodyToSend);

	return (_ret);
}

//-------------------------------------------------Error---------------------------------------

//-------------------------------------------------Other---------------------------------------

void	Header::printHeaderInfo(void)
{
	std::map<std::string, std::string>	map;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it1;

	map = _request.getHeaderFields();
	std::cout << PINK << "request method = " << _request.getMethod() << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _request.getURI() << ZERO_C << std::endl;
	std::cout << PINK << "host  = " << _request.getHost() << ZERO_C << std::endl;
	std::cout << PINK << "request query = " << _request.getQuery() << ZERO_C << std::endl;
	std::cout << PINK << "request http versioin = " << _request.getVersion() << ZERO_C << std::endl;
	std::cout << YELLOW << "request header:\n" << _buff << ZERO_C << std::endl;
	
	std::cout << TURGUOISE << "HEADER MAP" << ZERO_C << std::endl;
	for ( it = map.begin(); it != map.end() ; it++)
	{
		std::cout << PINK << it->first << BLUE << it->second << ZERO_C << std::endl;
		// std::cout << "1\n";
	}
	
}

void	Header::clear(void)
{
	_fd = -1;
	_buff = NULL;
	_ret = 200;
	_request.clear();
	_buff = NULL;
	_config = NULL;
}

Header::~Header()
{
}
