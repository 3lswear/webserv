#include "Request.hpp"

//-------------------------------------------------Constructors---------------------------------------

Request::Request()
{
    _row = 0;
    _ret = 200;
	_contentLength = 0;
	_chunked = false;
	_head_ok = false;
	_body_ok = false;
	_received = 0;
	_headerSize = 0;
	_lifeTime = 5;

}

Request::Request(char *str)
{
    _row = 0;
    _ret = 200;
    _data = str;
	_head_ok = false;
	_body_ok = false;
	_received = 0;
	_chunked = false;
	_contentLength = 0;
	_headerSize = 0;
	_lifeTime = 5;
}

//-------------------------------------------------Get/Set---------------------------------------

std::string					&Request::getURI(void)
{
    return (_URI);
}
std::string					&Request::getBody(void)
{
    return (_body);
}
std::string					&Request::getHost(void)
{
    return (_host);
}
std::string					&Request::getQuery(void)
{
    return(_query);
}
std::string					&Request::getMethod(void)
{
    return  (_method);
}
std::string					&Request::getFullUri(void)
{
    return (_fullURI);
}
std::string					&Request::getVersion(void)
{
    return (_version);
}
std::string					&Request::getLocation(void)
{
    return (_location);
}
std::string					&Request::getConnection(void)
{
	return (_connection);
}
ServerConfig				*Request::getConfig(void)
{
    return (_config);
}
int							Request::getCode(void)
{
    return (_ret);
}
int							Request::getLifeTime(void)
{
	return (_lifeTime);
}
std::map<std::string, std::string>	Request::getClientFields(void)
{
    return (_headerField);
}
bool						Request::getChunked(void)
{
	return (_chunked);
}

unsigned int Request::getContentLength(void) const
{
	return (_contentLength);
}
unsigned int Request::getHeaderSize(void) const
{
	return (_headerSize);
}
unsigned int Request::getRecved(void) const
{
	return (_received);
}
void                        Request::setData(char *str)
{
    this->_data = str;
}

void						Request::setData(std::string &str)
{
	_stringBUF = str;
}

void                        Request::setData(char *str, ServerConfig *config)
{
    _data = str;
    _config = config;
}
void                        Request::setConfig(ServerConfig *config)
{
    this->_config = config;
}

//-------------------------------------------------Parsing---------------------------------------

void	Request::increaseRecvCounter(unsigned int n)
{
	_received += n;
}
void	Request::parseURI(std::string str)
{
	std::string	tmp;
	int			pos;
	
	tmp = str;
	pos = str.find("?");
	if (pos > 0)
	{
		_URI = tmp.substr(0, pos);
	    _query = tmp.erase(0, pos + 1);
	}
	else
		_URI = str;
	_fullURI = HOME + _URI;
}

int	Request::parseStartLine(std::string str)
{
    std::string     tmp;

	_method = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	tmp = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	_version = str;
	_version.erase(_version.find_last_not_of(WHITESPACE) + 1);
	parseURI(tmp);

	if (_version != "HTTP/1.1")
		_ret =  505;
	return (_ret);
}

int								checkEnd(const std::string& str, const std::string& end)
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if (i < 0 || str[i] != end[j])
			return (1);
	}
	return (0);
}	

void	Request::splitData(std::string	&data)
{
	int	pos;
	std::string	str;

	str = std::string(data);
	if (!_head_ok)
	{
		pos = str.find("\r\n\r\n");
		if (pos == -1)
		{
			_ret = 400;
			return; 
		}
		_head = str.substr(0, pos) + "\n";
 		_headerSize = _head.size() + 3;
		data.erase(0, pos + 4);
		_head_ok = true;
		parseHeader();
		if (_contentLength == 0 && !_chunked)
			_body_ok = true;
	}
	if (badCode(_ret))
		return ;
	else if (_chunked)
	{
		_body.insert(_body.end(), data.begin(), data.end());
		if (checkEnd(_body, "0\r\n\r\n") == 0)
			_body_ok = true;
	}
	else if (!_body_ok)
	{

		_body.insert(_body.end(), data.begin(), data.end());
		if ((_received - _headerSize) == _contentLength)
		{
			_body_ok = true;
		}
	}
	if (_head_ok && _body_ok && _chunked)
	{
		std::string	&tmp = _body;
		std::string	subchunk = tmp.substr(0, 100);
		std::string	newBody = "";
		int			chunksize = strtol(subchunk.c_str(), NULL, 16);
		size_t 		i = 0;

		while (chunksize)
		{
			i = tmp.find("\r\n", i) + 2;
			newBody += tmp.substr(i, chunksize);
			i += chunksize + 2;
			subchunk = tmp.substr(i, 100);
			chunksize = strtol(subchunk.c_str(), NULL, 16);
		}
		_body = newBody;
	}
}

int	Request::parseClientfield(std::string str)
{
	int	distance;
	std::string key;
	std::string value;

	distance = str.find(":");
	if (distance < 0 && str != "\r")
		return 200;
	key = str.substr(0, distance);
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	value = str.erase(0, distance + 1);
	if (_headerField.find(key) != _headerField.end())
	{
		std::cout << RED << "ERROR: double Client-field" << ZERO_C << std::endl;
	}
	else
	{
		value = value.erase(0, value.find_first_not_of(WHITESPACE));
		value = value.substr(0, value.find_last_not_of(WHITESPACE) + 1);
		_headerField[key] = value;
	}
	return 200;
}

int	Request::parseHeader(void)
{
	std::string			line;
	std::stringstream	buffStream;
	
	buffStream << _head;
	_ret = 200;
	while (std::getline(buffStream, line, '\n') && !badCode(_ret))
	{
		if (_row == 0)
			_ret = parseStartLine(line);
		else
			_ret = parseClientfield(line);
		_row++;
	}
	if (!badCode(_ret))
	    copyFromMap();
	return (_ret);
}

int	Request::parseRequest(void)
{
	if (!_head_ok || !_body_ok)
		splitData(_stringBUF);

	return (_ret);
}

//-------------------------------------------------Utils---------------------------------------


void    Request::copyFromMap()
{
	std::map<std::string, std::string>::iterator it;
	int	pos = 0;
	
	//host
    it = _headerField.find("host");
	if (it != _headerField.end())
	{
		_host = it->second;
		pos = _host.find(":");
		if (pos != -1)
			_host = _host.substr(0, pos);
	}
	pos = 0;
	//content-lenght
	it = _headerField.find("content-length");
	if (it != _headerField.end())
		_contentLength = atoi(it->second.c_str());
	pos = 0;
	//chunked
	it = _headerField.find("transfer-encoding");
	if (it != _headerField.end())
	{
		pos = it->second.find("chunked");
		if ( pos != -1)
			_chunked = true;
	}
	pos = 0;
	//connection
	it = _headerField.find("connection");
	if (it != _headerField.end())
		_connection = it->second;
}


bool    Request::badCode(int code)
{
    if (code == 200)
        return (false);
    return true;
}

bool	Request::ok(void)
{
	return (_head_ok && _body_ok);
}

int	Request::isFile(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (-1);
		else if (s.st_mode & S_IFREG)
			return (0);
	}
	else
		return (-1);
	return (-1);
}

int	Request::isDir(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR)
			return (0);
		else if (s.st_mode & S_IFREG)
			return (-1);
	}
	else
		return (-1);
	return (-1);
}

void    Request::clear(void)
{
    _ret = 200;
    _row = 0;
    _URI = "";
    _body = "";
    _host = "";
    _query = "";
    _method = "";
    _fullURI = "";
    _version = "";
    _location = "";
	_head = "";
	_head_ok = false;
	_body_ok = false;
    _headerField.clear();
    _data = NULL;
    _config = NULL;
}

Request::~Request()
{
}
