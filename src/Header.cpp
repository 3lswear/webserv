#include "Header.hpp"

Header::Header()
{
	this->_row = 0;
}

Header::Header(char *str)
{
	this->_row = 0;
	this->_buff = str;
	parseRequest();
}
//-------------------------------------------------GET/SET---------------------------------------
std::map<std::string, std::string>	Header::getRequest(void)
{
	return (this->_request);
}

int	Header::getType(void)
{
	return (this->_type);
}

std::string	Header::getFileName(void)
{
	return _fileName;
}

void	Header::setRequest(char *str)
{
	this->_buff = str;
}
//--------------------------------------------------

void	Header::setFile(std::string str)
{
	std::string del = " ";
	int	pos;

	pos = str.find(del);
	str.erase(0, pos + del.length());
	_fileName = str.substr(0, str.find(del));
	if (_fileName == "/")
		_fileName = "www/index2.html";
	else
		_fileName.insert(0, HOME);
}

void	Header::identifyType(std::string str)
{
	if (str.compare(0,3, "GET") == 0)
		_type = GET;
	else if (str.compare(0,4, "POST") == 0)
		_type = POST;
	else if (str.compare(0,6, "DELETE") == 0)
		_type = DELETE;
}

void	Header::parseRequest(void)
{
	std::string			line;
	std::string			key;
	int					distance;
	std::stringstream	buffStream;
	
	buffStream << _buff;
	distance = 0;
	while (std::getline(buffStream, line, '\n'))
	{
		if (_row == 0)
		{
			identifyType(line);
			setFile(line);
		}
		else
		{
			distance = line.find(":");
			key = line.substr(0, distance);
			line = line.erase(0, distance + 1);
			if (_request.find(key) != _request.end())
			{
				std::cout << RED << "ERROR: double header-field" << ZERO_C << std::endl;
				std::cout << RED << (_request.find(key))->first << (_request.find(key))->second << std::endl;
			}
			else
				_request[key] = line;
		}
		_row++;
	}
}

void	Header::printHeaderInfo(void)
{
	std::map<std::string, std::string>::iterator it;

	std::cout << YELLOW << "request type = " << _type << ZERO_C << std::endl;
	std::cout << YELLOW << "request rows = " << _row << ZERO_C << std::endl;
	std::cout << YELLOW << "request fileName = " << _fileName << ZERO_C << std::endl;
	std::cout << YELLOW << "request header:\n" << _buff << ZERO_C << std::endl;
	
	std::cout << TURGUOISE << "HEADER MAP" << ZERO_C << std::endl;
	for ( it = _request.begin(); it != _request.end(); it++)
	{
		std::cout << PINK << it->first << ZERO_C << std::endl;
	}
	for ( it = _request.begin(); it != _request.end(); it++)
	{
		std::cout << PINK << it->second << ZERO_C << std::endl;
	}
	
}

void	Header::clearHeader(void)
{
	_type = -1;
	_row = 0;
	_buff = NULL;
	_fileName = "www/index2.html";
	_request.clear();
}

Header::~Header()
{
}
