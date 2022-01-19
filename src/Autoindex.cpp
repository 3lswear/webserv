#include "Autoindex.hpp"

Autoindex::Autoindex()
{
}

std::string Autoindex::getPage(std::string path, std::string host)
{
    std::string allpath = HOME + path;
    DIR *dir = opendir(allpath.c_str());
    struct dirent *dirEnt;
    std::string page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + allpath + "</title>\n\
    </head>\n\
    <body>\n\
    <h1>" + allpath + "</h1>\n\
    <p>\n";
    if (dir == NULL)
    {
        std::cerr << RED << "AutoindexError: could not open \"" 
            << allpath << "\" directory." << ZERO_C << std::endl;
        return "";
    }
    // if (allpath[0] != '/')
    //     path = "/" + path;
    for (dirEnt = readdir(dir); dirEnt; dirEnt = readdir(dir))
    {
        page = page + getReference(dirEnt->d_name, path, host);
    }
    page += "</p>\n</body>\n</html>\n";
    closedir(dir);
    return (page);
}

std::string Autoindex::getReference(std::string file, std::string dir, std::string host)
{
    std::stringstream link;
    link << "\t\t<p><a href=\"http://" << host
        << dir;
    if (dir[dir.length() - 1] != '/')
        link << "/";
    link << file << "\">" << file << "</a></p>\n";
    return link.str();
}

Autoindex::~Autoindex()
{
}