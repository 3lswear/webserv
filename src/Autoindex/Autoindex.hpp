#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include "webserv.hpp"

class Autoindex
{
private:
    
public:
    Autoindex();
    static std::string getPage(std::string path, std::string allpath, std::string host);
    static std::string getReference(std::string file, std::string dir, std::string host);
    ~Autoindex();
};




#endif