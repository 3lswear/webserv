#include "Autoindex.hpp"

Autoindex::Autoindex()
{
}

std::string Autoindex::getPage(std::string path, std::string allpath, std::string host, int port)
{
    DIR *dir = opendir(allpath.c_str());
    struct dirent *dirEnt;
    std::string     tmp;
    std::string page =\
    "<!DOCTYPE html>\n\
    <html>\n\
    <head>\n\
            <title>" + allpath + "</title>\n\
			<style>\n\
				p {font-family: monospace; font-size: 1.5em;}\n\
			</style>\n\
    </head>\n\
    <body>\n\
    <h1>" + allpath + "</h1>\n\
    <p>\n";
    if (dir == NULL)
    {
        DBOUT << WARNING << getDebugTime() << FAIL << "AutoindexError: could not open \"" 
            << allpath << "\" directory." << ZERO_C << std::endl;
        return "";
    }
    for (dirEnt = readdir(dir); dirEnt; dirEnt = readdir(dir))
    {
        tmp = dirEnt->d_name;
        if (tmp != ".." && tmp != ".")
            page = page + getReference(tmp, path, host, port);
    }
    page += "</p>\n</body>\n</html>\n";
    closedir(dir);
    return (page);
}

std::string Autoindex::getReference(std::string file, std::string dir, std::string host, int port)
{
    std::stringstream link;
    link << "\t\t<p><a href=\"http://" << host << ":" << port
        << dir;
    if (dir[dir.length() - 1] != '/')
        link << "/";
    link << file << "\">" << file << "</a></p>\n";
    return link.str();
}

Autoindex::~Autoindex()
{
}
