#include "Header.hpp"

Header::Header()
{
	this->_row = 0;

}

// std::string** Header::initErrorCode(void)
// {
// 		std::string HttpCode [41][2]= {{"100", "Continue"},
//                                                  {"101", "Switching Protocols"},
//                                                  {"200", "OK"},
//                                                  {"201", "Created"},
//                                                  {"202", "Accepted"},
//                                                  {"203", "Non-Authoritative Information"},
//                                                  {"204", "No Content"},
//                                                  {"205", "Reset Content"},
//                                                  {"206", "Partial Content"},
//                                                  {"300", "Multiple Choices"},
//                                                  {"301", "Moved Permanently"},
//                                                  {"302", "Found"},
//                                                  {"303", "See Other"},
//                                                  {"304", "Not Modified"},
//                                                  {"305", "Use Proxy"},
//                                                  {"306", "(Unused)"},
//                                                  {"307", "Temporary Redirect"},
//                                                  {"400", "Bad Request"},
//                                                  {"401", "Unauthorized"},
//                                                  {"402", "Payment Required"},
//                                                  {"403", "Forbidden"},
//                                                  {"404", "Not Found"},
//                                                  {"405", "Method Not Allowed"},
//                                                  {"406", "Not Acceptable"},
//                                                  {"407", "Proxy Authentication Required"},
//                                                  {"408", "Request Timeout"},
//                                                  {"409", "Conflict"},
//                                                  {"410", "Gone"},
//                                                  {"411", "Length Required"},
//                                                  {"412", "Precondition Failed"},
//                                                  {"413", "Request Entity Too Large"},
//                                                  {"414", "Request-URI Too Long"},
//                                                  {"415", "Unsupported Media Type"},
//                                                  {"416", "Requested Range Not Satisfiable"},
//                                                  {"417", "Expectation Failed"},
//                                                  {"500", "Internal Server Error"},
//                                                  {"501", "Not Implemented"},
//                                                  {"502", "Bad Gateway"},
//                                                  {"503", "Service Unavailable"},
//                                                  {"504", "Gateway Timeout"},
//                                                  {"505", "HTTP Version Not Supported"}};
// 	return (HttpCode);
// }

Header::Header(char *str)
{
	this->_row = 0;
	this->_buff = str;

	parseRequest();
}
//-------------------------------------------------GET/SET---------------------------------------
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

void	Header::setRawData(char *str)
{
	this->_buff = str;
}
//--------------------------------------------------

int	Header::parseStartLine(std::string str)
{
	std::string	tmp[3];

	_method = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	_URI = str.substr(0, str.find(" "));
	str = str.erase(0 , str.find(" ") + 1);
	_version = str;
	_version.erase(_version.find_last_not_of(" \n\r\t") + 1);
	if (_URI == "/")
		_URI = "www/index2.html";
	else
		_URI = HOME + _URI;
	if (_version != "HTTP/1.1")
		return (505);
	else if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return (405);
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
	int					ret;
	std::stringstream	buffStream;
	
	buffStream << _buff;
	ret = 200;
	while (std::getline(buffStream, line, '\n') && ret == 200)
	{
		if (_row == 0)
			ret = parseStartLine(line);
		else
			ret = parseHeaderfield(line);
		_row++;
	}
	return (200);
}

void	Header::printHeaderInfo(void)
{
	std::map<std::string, std::string>::iterator it;

	std::cout << PINK << "request method = " << _method << ZERO_C << std::endl;
	std::cout << PINK << "request URI = " << _URI << ZERO_C << std::endl;
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
