#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"

class Location
{
private:
    location    *loc;
    
public:
    Location();
    ~Location();

    void    setLocation(location *location);
    bool    checkLocation(std::string URI);
private:

};

#endif