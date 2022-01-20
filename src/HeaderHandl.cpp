#include "HeaderHandl.hpp"


HeaderHandl::HeaderHandl()
{

}

HeaderHandl::~HeaderHandl()
{

}

void    HeaderHandl::copyData(std::map<std::string, std::string> map)
{
    _host = map.find("host")->second;
}
