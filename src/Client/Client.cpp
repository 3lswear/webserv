#include "Client.hpp"

//-------------------------------------------------Constructors---------------------------------------

Client::Client()
{
	allRead = false;
	this->_fd = -1;
	this->_sended = 0;
}

Client::Client(char *str)
{
	allRead = false;
	this->_fd = -1;
	this->_buff = str;
	this->_sended = 0;

}

Client::Client(char *str, ServerConfig *config)
{
	allRead = false;
	this->_fd = -1;
	this->_config = config;
	this->_buff = str;
	this->_sended = 0;

}

//-------------------------------------------------GET/SET---------------------------------------

Request Client::getRequest(void)
{
	return (_request);
}

Response	Client::getResponse(void)
{
	return (_response);
}

std::string	Client::getStrToSend(void)
{
	return (_toSend);
}

int	Client::getFd(void)
{
	return _fd;
}

unsigned int Client::getCounter(void) const
{
	return _sended;
}

unsigned int Client::getRecvCounter(void) const
{
	return _request.getRecved();
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
//Возвращает истина, если запрос chunked.
bool Client::isChunked(void)
{
	return (_request.getChunked());
}
// Возвращает истина, если отправлены все данные клиента.
bool	Client::allSended(void)
{
	if (_toSend.size() <= _sended)
		return (true);
	return (false);
}

bool	Client::allRecved(void)
{
	if (_request.getContentLength() == (_request.getRecved() - _request.getHeaderSize()))
		return (true);
	else
		return (false);
}
// Функция увеличивает счетчик на количество BUFFERSIZE. Счетчик - количество байтов отправленных клиенту.
void	Client::increaseCounter(void)
{
	_sended += BUFFSIZE;
}

void 	Client::increaseRecvCounter(unsigned int n)
{
	_request.increaseRecvCounter(n);
}

//Генерирует response. Далее респонс можно получить через функцию getStrToSend()
int	Client::sendResponse(int	fd)
{
	_response.setData(_request, _config);
	_response.generate();
	_headerToSend = _response.getHeader();
	_bodyToSend = _response.getBody();
	_ret = sendData(fd, _headerToSend + _bodyToSend);

	return (_ret);
}

std::string	Client::generateRespons(void)
{
	_response.setData(_request, _config);
	_response.generate();
	_headerToSend = _response.getHeader();
	_bodyToSend = _response.getBody();
	_toSend = _headerToSend + _bodyToSend;

	return (_toSend);
}

//-------------------------------------------------Error---------------------------------------

//-------------------------------------------------Other---------------------------------------

bool	Client::readyToSend(void)
{
	return(_request.ok());
}

void	Client::printClientInfo(void)
{
	std::map<std::string, std::string>	map;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it1;

	map = _request.getClientFields();
	std::cout << TURQ << "Parsed request" << ZERO_C << std::endl;
	std::cout << PINK << "request method = " << _request.getMethod() << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _request.getURI() << ZERO_C << std::endl;
	std::cout << PINK << "host  = " << _request.getHost() << ZERO_C << std::endl;
	std::cout << PINK << "request query = " << _request.getQuery() << ZERO_C << std::endl;
	std::cout << PINK << "request http versioin = " << _request.getVersion() << ZERO_C << std::endl;
	std::cout << PINK << "content-length = " << _request.getContentLength() << ZERO_C << std::endl;
	std::cout << PINK << "connection type = " << _request.getConnection() << ZERO_C << std::endl;

	std::cout << TURQ << "Parsed response" << ZERO_C << std::endl;
	std::cout << BLUE << _response.getHeader() << ZERO_C << std::endl;
	
	// std::cout << YELLOW << "request Client:\n" << _buff << ZERO_C << std::endl;
	// std::cout << TURGUOISE << "Client MAP" << ZERO_C << std::endl;
	// for ( it = map.begin(); it != map.end() ; it++)
	// {
	// 	std::cout << PINK << it->first << BLUE << it->second << ZERO_C << std::endl;
	// }
	// std::cout << TURGUOISE << "Client BODY" << ZERO_C << std::endl;
	// std::cout << GREEN << _request.getBody().size() << ZERO_C << std::endl;
	/* std::cout << BLUE << _request.getBody() << ZERO_C << std::endl; */
	
}

void	Client::clear(void)
{
	_fd = -1;
	_buff = NULL;
	_ret = 200;
	_request.clear();
	_buff = NULL;
	_config = NULL;
	_bodyToSend = "";
	_headerToSend = "";
	_toSend = "";
}

Client::~Client()
{
}
