#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include "webserv.hpp"

class ConfigException
{
private:
    std::string _msg;
public:
    ConfigException(std::string const &str) : _msg(str){}
    ~ConfigException(){}
    std::string getMessage() const {return (_msg);}
};


#endif