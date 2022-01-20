#ifndef HEADERHANDL_HPP
#define HEADERHANDL_HPP

#include <iostream>
#include <map>

class HeaderHandl
{
public:
    std::string _method;
    std::string _URI;
    std::string _fullURI;
    std::string _version;
    std::string _query;
public:
    // std::string _allow;
    // std::string _contentLenght;
    std::string _contentType;
    std::string _host;
    // std::string _contentLanguage;
    std::string _contentLocation;
    // std::string _date;
    // std::string _lastModified;
    // std::string _transferEncoding;
    // std::string _server;
    // std::string _location;
public:
    HeaderHandl();
    
    void    copyData(std::map<std::string, std::string>);

    ~HeaderHandl();
};



#endif