#include "Header.hpp"

#define WHITESPACE " \n\r\t\f\v"

Header::Header()
{
	this->_row = 0;
	this->_fd = -1;
	this->_autoIndex = 1;
	initErrorCode();

}

Header::Header(char *str)
{
	this->_row = 0;
	this->_fd = -1;
	this->_buff = str;
	this->_autoIndex = 1;

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

std::map<std::string, std::string>	Header::getHeaderField(void)
{
	return (this->_headerField);
}

HeaderHandl Header::getRequest(void)
{
	return (_request);
}

HeaderHandl	Header::getRespons(void)
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

void	Header::parseURI(std::string str)
{
	std::string	tmp;
	int			pos;
	
	tmp = str;
	pos = str.find("?");
	if (pos > 0)
	{
		_request._URI = tmp.substr(0, pos);
		_request._query = tmp.erase(0, pos + 1);
	}
	else
		_request._URI = str;
	_request._fullURI = HOME + _request._URI;
}

int	Header::parseStartLine(std::string str)
{
	std::string	tmp[3];

	tmp[0] = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	tmp[1] = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	tmp[2] = str;
	tmp[2].erase(tmp[2].find_last_not_of(" \n\r\t") + 1);
	_request._method = tmp[0];
	parseURI(tmp[1]);
	_request._version = tmp[2];

	if (_request._version != "HTTP/1.1")
		_ret =  505;
	else if (_request._method != "GET" && _request._method != "POST"
			&& _request._method != "DELETE")
		_ret =  405;
	else if (isFile(_request._fullURI) != 0)
	{
		if (isDir(_request._fullURI) == 0 && _autoIndex)
			_ret = 200;
		else if (_autoIndex)
			_ret = 404;
		else
			_ret = 403;
	}
	return (_ret);
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
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	value = str.erase(0, distance + 1);
	if (_headerField.find(key) != _headerField.end())
	{
		std::cout << RED << "ERROR: double header-field" << ZERO_C << std::endl;
	}
	else
	{
		value = value.erase(0, value.find_first_not_of(WHITESPACE));
		value = value.substr(0, value.find_last_not_of(WHITESPACE) + 1);
		_headerField[key] = value;
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
	if (_ret == 200)
		_request.copyData(_headerField);
	return (_ret);
}

//-------------------------------------------------FILE---------------------------------------

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

	ss << _request._version << " " << _ret << " " << getReasonPhrase(_ret) << "\r\nContent-Type: text/html\r\n\r\n";
	tmp = ss.str();
	header = tmp.c_str();
	std::cout << TURGUOISE << "Send Header\n" << YELLOW << tmp << ZERO_C;
	send(fd, header, tmp.length(), 0);
	return (0);
}

int	Header::sendRespons(int fd)
{
	std::string path;

	path = _request._fullURI;
	sendHeader(fd);
	if (_ret == 200 && isDir(path) == 0)
		_fileToSend = Autoindex::getPage(_request._URI, _request._host);
	else if (_ret == 200)
		OpenResponsFile(path.c_str());
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

	std::cout << PINK << "request method = " << _request._method << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _request._URI << ZERO_C << std::endl;
	std::cout << PINK << "host  = " << _request._host << ZERO_C << std::endl;
	std::cout << PINK << "request query = " << _request._query << ZERO_C << std::endl;
	std::cout << PINK << "request http versioin = " << _request._version << ZERO_C << std::endl;
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
	_request._method = "";
	_row = 0;
	_buff = NULL;
	_request._URI = "";
	_request._URI = "";
	_headerField.clear();
}

Header::~Header()
{
}
