#include "Client.hpp"

//-------------------------------------------------Constructors---------------------------------------

Client::Client(struct serverListen servers_config)
{
	allRead = false;
	done = false;
	this->_fd = -1;
	this->_sended = 0;
	_to_send_char = NULL;

	connected_to = servers_config;
}

Client::Client(char *str)
{
	allRead = false;
	done = false;
	this->_fd = -1;
	this->_buff = str;
	this->_sended = 0;
	_to_send_char = NULL;


}

Client::Client(char *str, ServerConfig *config)
{
	_to_send_char = NULL;
	allRead = false;
	done = false;
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

char	*Client::getStrToSend(void)
{
	return (_to_send_char);
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

void	Client::setRawData(std::string	&buf)
{
	_stringBUF = buf;
}

void	Client::setFd(int fd)
{
	this->_fd = fd;
}

//-------------------------------------------------Parsing---------------------------------------

int	Client::parseRequest(void)
{
	// _request.setData(_buff);
	_request.setData(_stringBUF);
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
	if (response_len <= _sended)
		done = true;
	return (done);
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
// int	Client::sendResponse(int	fd)
// {
// 	_response.setData(_request, _config);
// 	_response.generate();
// 	_headerToSend = _response.getHeader();
// 	_bodyToSend = _response.getBody();
// 	_ret = sendData(fd, _headerToSend + _bodyToSend);

// 	return (_ret);
// }

// std::string	Client::generateRespons(void)
// {
// 	size_t len;

// 	_response.setData(_request, _config);
// 	_response.generate();
// 	_headerToSend = _response.getHeader();
// 	_bodyToSend = _response.getBody();
// 	*_toSend = _headerToSend + _bodyToSend;


// 	len = _toSend->size();
// 	response_len = len;
// 	_to_send_char = new char[len + 1];
// 	std::memcpy(_to_send_char, _toSend->c_str(), len + 1);

// 	return (*_toSend);
// }

std::string	Client::generateRespons(std::vector<ServerConfig *> &configs)
{
	size_t		len;
	std::vector<location *> tmp;

	_config	=	Config::getConfig(configs, _request, connected_to);
	tmp 	=	Config::getLocation(_config->getLocations(), _request.getURI());
	_response.setData(_request, _config, tmp);
	_response.generate2(connected_to);

	_toSend = new std::string;
	*_toSend = (_response.getBody() == NULL) ? _response.getHeader() : _response.getHeader() + *_response.getBody();


	len = _toSend->size();
	response_len = len;
	_to_send_char = new char[len + 1];
	std::memcpy(_to_send_char, _toSend->c_str(), len + 1);

	//print request
	std::cerr << WARNING << getDebugTime() << TURGUOISE;
	if (_request.getCode() != 200)
		std::cerr  << FAIL ;
	std::cerr << "[ request  ] : "
		<<	"[method: " << _request.getMethod() << "] "
		<<	"[URI: "  << _request.getURI() << " ]";
	if (_response.getLocation() != NULL)
		std::cerr << " [location: " << _response.getLocation()->location << " ]";
	if (_config != NULL && !_config->getServerName().empty())
		std::cerr << " [server: " << _config->getServerName() << " ]" << ENDL;
	else
		std::cerr << ENDL;
	//print response
	std::cerr << WARNING << getDebugTime() << GREEN;
	if (_response.getCode() >= 400)
		std::cerr << FAIL ;
	std::cerr << "[ response ] : " << "[status: " << _response.getCode() 
		<< " " << _response.getReasonPhrase(_response.getCode())
		<< " ] [Content-Length: " << _response.getBodySize() << " ]";
	if (_response.isRedirect())
		std::cerr << " [redirect: " << _response.getRedirect() << " ]" << ENDL;
	else
		std::cerr << ENDL;

	delete _toSend;
	_request.freeData();
	_response.freeData();
	return (_headerToSend);
}

//-------------------------------------------------Error---------------------------------------

//-------------------------------------------------Other---------------------------------------

bool	Client::readyToSend(void)
{
	if (_request.ok())
		return(_request.ok());
	else 
		return false;
}

void	Client::printClientInfo(void)
{
	std::map<std::string, std::string>	map;
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string>::iterator it1;

	map = _request.getClientFields();
	std::cout << BLUE << std::endl << "PARSED REQUEST" << ZERO_C << std::endl << std::endl;
	std::cout << GREEN << "request method = " << _request.getMethod() << ZERO_C << std::endl;
	std::cout << GREEN << "request URI = " << _request.getURI() << ZERO_C << std::endl;
	std::cout << GREEN << "host  = " << _request.getHost() << ZERO_C << std::endl;
	std::cout << GREEN << "request query = " << _request.getQuery() << ZERO_C << std::endl;
	std::cout << GREEN << "request http versioin = " << _request.getVersion() << ZERO_C << std::endl;
	std::cout << GREEN << "content-length = " << _request.getContentLength() << ZERO_C << std::endl;
	std::cout << GREEN << "connection type = " << _request.getConnection() << ZERO_C << std::endl;
	it = map.find("transfer-encoding");
	if (it != map.end())
		std::cout << GREEN << "transfer-encoding = " << it->second << ZERO_C << std::endl;


	std::cout << BLUE << std::endl << "RESPONSE" << ZERO_C << std::endl << std::endl;
	std::cout << GREEN << _response.getHeader() << ZERO_C << std::endl;
}

bool	Client::isEmpty(void)
{
	if (!_request.ok() && _request.getHeaderSize() == 0
		&& _request.getContentLength() == 0)
		return (true);
	else
		return (false);
}

bool	Client::TimeToDie(void)
{
	struct timeval curTime;

	gettimeofday(&curTime, NULL);
	if ((curTime.tv_sec - _time.tv_sec) >= _request.getLifeTime())
		return (true);
	else
		return (false);
}

serverListen	Client::getIpPort()
{
	return (connected_to);
}

void	Client::updateTimeToDie(void)
{
	gettimeofday(&_time, NULL);
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
	if (_to_send_char)
	{
		delete[] _to_send_char;
		_to_send_char = NULL;
	}
	_request.freeData();
	_response.freeData();
}

Client::~Client()
{
}
