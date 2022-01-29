#include "Request.hpp"

//-------------------------------------------------Constructors---------------------------------------

Request::Request()
{
    _row = 0;
    _ret = 200;
}

Request::Request(char *str)
{
    _row = 0;
    _ret = 200;
    _data = str;
}

//-------------------------------------------------Get/Set---------------------------------------

std::string					Request::getURI(void)
{
    return (_URI);
}
std::string					Request::getBody(void)
{
    return (_body);
}
std::string					Request::getHost(void)
{
    return (_host);
}
std::string					Request::getQuery(void)
{
    return(_query);
}
std::string					Request::getMethod(void)
{
    return  (_method);
}
std::string					Request::getFullUri(void)
{
    return (_fullURI);
}
std::string					Request::getVersion(void)
{
    return (_version);
}
std::string					Request::getLocation(void)
{
    return (_location);
}
ServerConfig				*Request::getConfig(void)
{
    return (_config);
}
int							Request::getCode(void)
{
    return (_ret);
}
std::map<std::string, std::string>	Request::getClientFields(void)
{
    return (_ClientField);
}

void                        Request::setData(char *str)
{
    this->_data = str;
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
	else if (_method != "GET" && _method != "POST"
			&& _method != "DELETE")
		_ret =  405;
	else if (isFile(_fullURI) != 0 && isDir(_fullURI) != 0)
        _ret = 404;
	return (_ret);
}

int	Request::parseClientfield(std::string str)
{
	int	distance;
	std::string key;
	std::string value;

	distance = str.find(":");
	if (distance < 0 && str != "\r")
		return 400;
	key = str.substr(0, distance);
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	value = str.erase(0, distance + 1);
	if (_ClientField.find(key) != _ClientField.end())
	{
		std::cout << RED << "ERROR: double Client-field" << ZERO_C << std::endl;
	}
	else
	{
		value = value.erase(0, value.find_first_not_of(WHITESPACE));
		value = value.substr(0, value.find_last_not_of(WHITESPACE) + 1);
		_ClientField[key] = value;
	}
	return 200;
}

int	Request::parseRequest(void)
{
	std::string			line;
	std::stringstream	buffStream;
	
	buffStream << _data;
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

//-------------------------------------------------Utils---------------------------------------


void    Request::copyFromMap()
{
    _host = _ClientField.find("host")->second;
}

bool    Request::badCode(int code)
{
    if (code == 200)
        return (false);
    return true;
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
    _ClientField.clear();
    _data = NULL;
    _config = NULL;
}

Request::~Request()
{
}
