#include "Header.hpp"

Header::Header()
{
	this->_row = 0;
	this->_fd = -1;
	initErrorCode();

}

Header::Header(char *str)
{
	this->_row = 0;
	this->_fd = -1;
	this->_buff = str;

	initErrorCode();
	parseRequest();
}

//-------------------------------------------------GET/SET---------------------------------------


std::string Header::getErrorPage(int code)
{
	std::stringstream	ss;
	std::string 		Page;

	ss << "<html><head><title>" << code <<" "<< getReasonPhrase(code) <<"</title></head><body>"
		<<"<center><h1>" << code <<" " << getReasonPhrase(code) <<"</h1></center> "
		<< "<hr><center>poheck/1.0.0 (KDE)</center></body></html>";
	Page = ss.str();
	return (Page);
}

std::map<std::string, std::string>	Header::getRequest(void)
{
	return (this->_headerField);
}

std::string	Header::getMethod(void)
{
	return (this->_method);
}

std::string	Header::getURI(void)
{
	return _URI;
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

int	Header::parseStartLine(std::string str)
{
	std::string	tmp[3];

	_method = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	_URI = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	_version = str;
	_version.erase(_version.find_last_not_of(" \n\r\t") + 1);
	if (_version != "HTTP/1.1")
		return (505);
	else if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return (405);
	else if (checkURI() < 0)
	{
		if (isDir(_URI) == 0)
			return (403);
		else
			return (404);
	}
	return 200;
}

int	Header::parseHeaderfield(std::string str)
{
	int	distance;
	std::string key;
	std::string value;

	distance = str.find(":");
	if (distance < 0 && str != "\r")
		return 400;
	key = str.substr(0, distance);
	value = str.erase(0, distance + 1);
	if (_headerField.find(key) != _headerField.end())
	{
		std::cout << RED << "ERROR: double header-field" << ZERO_C << std::endl;
	}
	else
	{
		_headerField[key] = value.erase(value.find_last_not_of(" \n\r\t") + 1);
	}
	return 200;
}

int	Header::parseRequest(void)
{
	std::string			line;
	std::stringstream	buffStream;
	
	buffStream << _buff;
	_ret = 200;
	while (std::getline(buffStream, line, '\n') && _ret == 200)
	{
		if (_row == 0)
			_ret = parseStartLine(line);
		else
			_ret = parseHeaderfield(line);
		_row++;
	}
	return (_ret);
}

//-------------------------------------------------FILE---------------------------------------

int	Header::checkURI(void)
{
	const char	*path;
	std::string	tmp;
	int			pos;

	tmp = _URI;
	pos = _URI.find("?");
	if (pos > 0)
	{
		_URI = tmp.substr(0, pos);
		_query = tmp.erase(0, pos + 1);
	}
	_URI = HOME + _URI;
	path = _URI.c_str();
	if (isFile(_URI) < 0)
		return (-1);
	return (0);
}

int	Header::isFile(std::string path)
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

int	Header::isDir(std::string path)
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

void Header::OpenResponsFile(const char *path)
{
	std::stringstream	ss;
	char 				buf[BUFFSIZE + 1] = {0};
	std::ifstream		file(path);

	if (file.is_open())
	{	
		while (!file.eof())
		{
			file.read(buf, BUFFSIZE);
			ss << buf;
			memset(buf, 0, BUFFSIZE + 1);
		}
		_fileToSend = ss.str();
		file.close();
	}
	else
		_fileToSend = getErrorPage(403); 	
}

//-------------------------------------------------SEND---------------------------------------

int	Header::sendHeader(int fd)
{
	std::stringstream ss;
	std::string tmp;
	const char *header;

	ss << _version << " " << _ret << " " << getReasonPhrase(_ret) << "\r\nContent-Type: text/html\r\n\r\n";
	tmp = ss.str();
	header = tmp.c_str();
	std::cout << TURGUOISE << "Send Header\n" << YELLOW << tmp << ZERO_C;
	send(fd, header, tmp.length(), 0);
	return (0);
}

int	Header::sendRespons(int fd)
{
	sendHeader(fd);
	if (_ret == 200)
		OpenResponsFile(_URI.c_str());
	else
		_fileToSend = getErrorPage(_ret);
	send(fd, _fileToSend.c_str(), _fileToSend.length(), 0);

	return (0);
}

//-------------------------------------------------Error---------------------------------------

void  Header::initErrorCode(void)
{
	_errorCode["100"]  = "Continue";
	_errorCode["101"]  = "Switching Protocols";
	_errorCode["200"]  = "OK";
	_errorCode["201"]  = "Created";
	_errorCode["202"]  = "Accepted";
	_errorCode["203"]  = "Non-Authoritative Information";
	_errorCode["204"]  = "No Content";
	_errorCode["205"]  = "Reset Content";
	_errorCode["206"]  = "Partial Content";
	_errorCode["300"]  = "Multiple Choices";
	_errorCode["301"]  = "Moved Permanently";
	_errorCode["302"]  = "Found";
	_errorCode["303"]  = "See Other";
	_errorCode["304"]  = "Not Modified";
	_errorCode["305"]  = "Use Proxy";
	_errorCode["306"]  = "(Unused)";
	_errorCode["307"]  = "Temporary Redirect";
	_errorCode["400"]  = "Bad Request";
	_errorCode["401"]  = "Unauthorized";
	_errorCode["402"]  = "Payment Required";
	_errorCode["403"]  = "Forbidden";
	_errorCode["404"]  = "Not Found";
	_errorCode["405"]  = "Method Not Allowed";
	_errorCode["406"]  = "Not Acceptable";
	_errorCode["407"]  = "Proxy Authentication Required";
	_errorCode["408"]  = "Request Timeout";
	_errorCode["409"]  = "Conflict";
	_errorCode["410"]  = "Gone";
	_errorCode["411"]  = "Length Required";
	_errorCode["412"]  = "Precondition Failed";
	_errorCode["413"]  = "Request Entity Too Large";
	_errorCode["414"]  = "Request-URI Too Long";
	_errorCode["415"]  = "Unsupported Media Type";
	_errorCode["416"]  = "Requested Range Not Satisfiable";
	_errorCode["417"]  = "Expectation Failed";
	_errorCode["500"]  = "Internal Server Error";
	_errorCode["501"]  = "Not Implemented";
	_errorCode["502"]  = "Bad Gateway";
	_errorCode["503"]  = "Service Unavailable";
	_errorCode["504"]  = "Gateway Timeout";
	_errorCode["505"]  = "HTTP Version Not Supported";
}

std::string	Header::getReasonPhrase(std::string code)
{
	std::map<std::string, std::string>::iterator it;

	it = _errorCode.find(code);
	return (it->second);
}

std::string	Header::getReasonPhrase(int code)
{
	std::stringstream ss;
	std::string nbr;
	std::map<std::string, std::string>::iterator it;

	ss << code;
	nbr = ss.str();
	it = _errorCode.find(nbr);
	return (it->second);
}

//-------------------------------------------------Other---------------------------------------

void	Header::printHeaderInfo(void)
{
	std::map<std::string, std::string>::iterator it;

	std::cout << PINK << "request method = " << _method << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _URI << ZERO_C << std::endl;
	std::cout << PINK << "request query = " << _query << ZERO_C << std::endl;
	std::cout << PINK << "request http versioin = " << _version << ZERO_C << std::endl;
	std::cout << PINK << "request rows = " << _row << ZERO_C << std::endl;
	std::cout << YELLOW << "request header:\n" << _buff << ZERO_C << std::endl;
	
	std::cout << TURGUOISE << "HEADER MAP" << ZERO_C << std::endl;
	for ( it = _headerField.begin(); it != _headerField.end(); it++)
	{
		std::cout << PINK << it->first << BLUE << it->second << ZERO_C << std::endl;
	}
	
}

void	Header::clear(void)
{
	_fd = -1;
	_method = "";
	_row = 0;
	_buff = NULL;
	_URI = "";
	_version = "";
	_headerField.clear();
}

Header::~Header()
{
}
