#include "Client.hpp"

//-------------------------------------------------Constructors---------------------------------------

Client::Client()
{
	this->_fd = -1;
}

Client::Client(char *str)
{
	this->_fd = -1;
	this->_buff = str;

}

Client::Client(char *str, ServerConfig *config)
{
	this->_fd = -1;
	this->_config = config;
	this->_buff = str;

}

//-------------------------------------------------GET/SET---------------------------------------

Request Client::getRequest(void)
{
	return (_request);
}

Response	Client::getResponse(void)
{
	return (_Response);
}

int	Client::getFd(void)
{
	return _fd;
}

void	Client::setRawData(char *str)
{
	this->_buff = str;
}

void	Client::setFd(int fd)
{
	this->_fd = fd;
}

//-------------------------------------------------Parsing---------------------------------------

int	Client::parseRequest(void)
{
	_request.setData(_buff);
	_ret = _request.parseRequest();

	return (_ret);
}

//-------------------------------------------------FILE---------------------------------------


//-------------------------------------------------SEND---------------------------------------

int	Client::sendData(int fd, std::string data)
{
	return (send(fd, data.c_str(), data.length(), 0));
}

int	Client::sendResponse(int	fd)
{
	_Response.setData(_request, _config);
	_Response.generate();
	_ClientToSend = _Response.getClient();
	_bodyToSend = _Response.getBody();
	_ret = sendData(fd, _ClientToSend);
	_ret = sendData(fd, _bodyToSend);

	return (_ret);
}

//-------------------------------------------------Error---------------------------------------

//-------------------------------------------------Other---------------------------------------

void	Client::printClientInfo(void)
{
	std::map<std::string, std::string>	map;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it1;

	map = _request.getClientFields();
	std::cout << PINK << "request method = " << _request.getMethod() << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _request.getURI() << ZERO_C << std::endl;
	std::cout << PINK << "host  = " << _request.getHost() << ZERO_C << std::endl;
	std::cout << PINK << "request query = " << _request.getQuery() << ZERO_C << std::endl;
	std::cout << PINK << "request http versioin = " << _request.getVersion() << ZERO_C << std::endl;
	std::cout << YELLOW << "request Client:\n" << _buff << ZERO_C << std::endl;
	
	std::cout << TURGUOISE << "Client MAP" << ZERO_C << std::endl;
	for ( it = map.begin(); it != map.end() ; it++)
	{
		std::cout << PINK << it->first << BLUE << it->second << ZERO_C << std::endl;
		// std::cout << "1\n";
	}
	
}

void	Client::clear(void)
{
	_fd = -1;
	_buff = NULL;
	_ret = 200;
	_request.clear();
	_buff = NULL;
	_config = NULL;
}

Client::~Client()
{
}
