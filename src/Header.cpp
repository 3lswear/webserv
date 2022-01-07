#include "Header.hpp"

Header::Header()
{
	this->_row = 0;
}

Header::Header(char *str)
{
	this->_row = 0;
	this->_buff = str;
}
//-------------------------------------------------GET/SET---------------------------------------
std::vector<std::string>	Header::getRequest(void)
{
	return (this->_request);
}
// std::string	Header::getRespons(void)
// {
// 	return (this->_respons);
// }
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

void	Header::parseBuff(void)
{
	std::string			line;
	std::stringstream	buffStream;
	
	buffStream << _buff;
	while (std::getline(buffStream, line, '\n'))
	{
		if (_row == 0)
		{
			identifyType(line);
			setFile(line);
		}
		_request.push_back(line);
		_row++;
	}
}

void	Header::printHeaderInfo(void)
{
	std::cout << YELLOW << "request type = " << _type << ZERO_C << std::endl;
	std::cout << YELLOW << "request rows = " << _row << ZERO_C << std::endl;
	std::cout << YELLOW << "request fileName = " << _fileName << ZERO_C << std::endl;
	std::cout << YELLOW << "request header:\n" << _buff << ZERO_C << std::endl;
}

Header::~Header()
{
}
