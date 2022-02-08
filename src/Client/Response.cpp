#include "Response.hpp"

//-------------------------------------------------Constructor---------------------------------------

Response::Response()
{
	initErrorCode();
	_Autoindex = true;
	_code = 200;
}

//-------------------------------------------------GET/SET---------------------------------------

std::string     Response::getHeader(void)
{
	return (_header);
}
std::string     Response::getBody(void)
{
	return (_body);
}

void            Response::setData(Request request, ServerConfig *config)
{
	_request = request;
	_code = request.getCode();
	_config = config;
}

void            Response::setData(Request &request, ServerConfig *config, location *loc)
{
	_request = request;
	_code = request.getCode();
	_config = config;
	_location = loc;
}

void		Response::setHeaderBlocks(void)
{
	setContentType();
    setContentLength();
    setServer();
    setConnection();
    setDate();
    setCacheControl();
    // setLocation(void);
    // setLanguage(void);
    // setTransferEncoding(void);
}
void	Response::setContentType(void)
{
	if (_code == 204)
		return ;
	else if (_request.badCode(_code))
		_contentType = "text/html";
	else
		_contentType = getContentType();
}

void	Response::setContentLength()
{
	_contentLength = _body.size();
}

void	Response::setServer(void)
{
	_server = "Poheck/1.0";
}

void	Response::setConnection()
{
	std::stringstream ss;
	if (_request.getConnection() == "keep-alive")
	{
		ss << "timeout=" << _request.getLifeTime();
		_keepAlive = ss.str();
	}
}

void	Response::setDate(void)
{
	_date = getTime();
}

void	Response::setCacheControl(void)
{
	_cacheControl = "no-store, no-cache, must-revalidate";
}

//-------------------------------------------------File---------------------------------------

void Response::OpenResponseFile(const char *path)
{
	std::stringstream	ss;
	std::ifstream		file(path, std::ifstream::in);

	if (file.is_open())
	{	
		ss << file.rdbuf();
		_body = ss.str();
		file.close();
	}
	else
	{
		_code = 403;
		OpenErrorFile(_code);
	}
}

void Response::OpenErrorFile(int code)
{
	std::map<int, std::string>::iterator it;
	std::stringstream	ss;

	it = _errorPages.find(code);
	if (it != _errorPages.end())
	{
		const char *path = it->second.c_str();
		std::ifstream		file(path, std::ifstream::in);

		if (file.is_open())
		{	
			ss << file.rdbuf();
			_body = ss.str();
			file.close();
		}
		else
			_body = getErrorPage(code);
	}
	else
		_body = getErrorPage(code);
}

std::string		Response::getContentType(void)
{
	std::string	path = _fullURI;
	std::string	type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));

	if (_request.isDir(path) == 0)
		return ("text/html");
	else if (type == "html")
		return ("text/html");
	else if (type == "css")
		return ("text/css");
	else if (type == "js")
		return ("text/javascript");
	else if (type == "json")
		return ("application/json");
	else if (type == "woff")
		return ("application/x-font-woff");
	else if (type == "mp4" || type == "m4v" || type == " f4v")
		return ("video/mp4");
	else if (type == "ico")
		return ("image/x-icon");
	else if (type == "jpeg" || type == "jpg")
		return ("image/jpeg");
	else if (type == "png")
		return ("image/png");
	else if (type == "bmp")
		return ("image/bmp");
	else if (type == "pdf")
		return ("application/pdf");
	else
		return ("text/plain");

}

std::string		Response::getTime(void)
{
	char	buff[1337] = {0};
	struct timeval	currTime;
	struct tm		*t;

	gettimeofday(&currTime, NULL);
	t = gmtime(&currTime.tv_sec);
	strftime(buff, 1337, "%a, %d %b %Y %H:%M:%S GMT", t);

	return (buff);
}

std::string	Response::getFullURI(void)
{
	std::string	tmp;
	bool end = false;
	std::string	ret = "";

	if (!_location->directoryFile.empty())
	{
    	struct dirent *dirEnt;
    	DIR *dir = opendir(_location->root.c_str());

		if (dir == NULL)
		{
			_code = 404;
			return "";
		}
		for (dirEnt = readdir(dir); !end && dirEnt; dirEnt = readdir(dir))
		{
			tmp = dirEnt->d_name;
			if (tmp == _location->directoryFile)
			{
				ret = tmp;
				end = true;
			}
		}
	}
	if (!end)
	{
		ret = _location->root;
	}
	return (ret);	
}

void	Response::generateBody(void)
{
	if (!_request.badCode(_code) && _request.isDir(_fullURI) == 0)
	{
		if (_Autoindex)
		{
			_body = Autoindex::getPage(_request.getURI(), _fullURI, _request.getHost());
			if (_body.empty())
				_code = 404;
		}
		else
			_code = 403;
	}
	else if (!_request.badCode(_code) && _request.isFile(_request.getFullUri()) == 0)
		OpenResponseFile(_fullURI.c_str());
	else if (_request.isFile(_fullURI) == -1)
		_code = 404;
	
	if (_request.badCode(_code))
		OpenErrorFile(_code);

}

bool	Response::allowedMethod(std::string &method)
{
	DBOUT << "allowedMethod called" << ENDL;
	std::vector<std::string>::iterator	it;

	it = _location->methods.begin();

	while (it != _location->methods.end())
	{
		if (*it == method)
			return (true);
		it++;
	}
	_code = 405;
	return (false);
	
}
void	Response::generateHeader(void)
{
	std::stringstream ss;
	std::string tmp;

	ss << "HTTP/1.1" << " " << _code << " " << getReasonPhrase(_code) << "\r\n";
	if (!_contentType.empty())
		ss << "Content-Type: " << _contentType << "\r\n";
	ss << "Content-Length: " << _contentLength << "\r\n";
	ss << "Server: " << _server << "\r\n";
	if (!_keepAlive.empty())
		ss << "Keep-Alive: " <<_keepAlive << "\r\n";
	ss << "Date: " << _date << "\r\n";
	if (!_cacheControl.empty())
		ss << "Cache-Control: " << _cacheControl << "\r\n";
	ss << "\r\n";
	_header = ss.str();
}

void    Response::generate()
{
	_fullURI = _request.getFullUri();
	if (_request.badCode(_request.getCode()))
		invalidClient();
	else if (_request.getMethod() == "GET")
		methodGet();
	else if (_request.getMethod() == "DELETE")
		methodDelete();
	else
		methodPost();
}

void	Response::generate2(void)
{
	_errorPages = _config->getErrorPages();
	_Autoindex	= _location->autoindex;
	_code = _request.getCode();
	_hostPort.ip = _config->getHost();
	_hostPort.port = _config->getPort();
	_fullURI = getFullURI();
	DBOUT << "fullURI " << _fullURI << ENDL;
	_method = _request.getMethod();

	DBOUT << RED << "code is " << _code << ENDL;

	if (_request.badCode(_code) || !allowedMethod(_method))
	{
		invalidClient();
		return;
	}
	if (_method == "GET")
		methodGet();
	else if (_method == "POST")
		methodPost();
	else
		methodDelete();
	
}

//-------------------------------------------------HEADER/BODY---------------------------------------


void	Response::invalidClient(void)
{
	OpenErrorFile(_code);
	setHeaderBlocks();
	generateHeader();

	DBOUT << RED << "Error Method called" << ENDL;
}


void	Response::methodGet(void)
{
	generateBody();
	setHeaderBlocks();
	generateHeader();
	std::cout << GREEN << "GET method called\n" << ZERO_C;
}
void	Response::methodPost(void)
{
	std::ofstream outfile(_fullURI.c_str(), std::ios::out | std::ios::binary);

	outfile.write(_request.getBody().data(), _request.getBody().size());
	_code = 204;
	setHeaderBlocks();
	generateHeader();
	DBOUT << GREEN << "POST method called" << ENDL;
}
void	Response::methodDelete(void)
{
	if (_request.isFile(_fullURI) == 0)
	{
		if (remove(_fullURI.c_str()) == 0)
			_code = 204;
		else
			_code = 403;
	}
	else
		_code = 404;
	if (_code == 404 || _code == 403)
		OpenErrorFile(_code);
	setHeaderBlocks();
	generateHeader();
	DBOUT << GREEN << "Delete method called" <<  ENDL;
}

//-------------------------------------------------GET/SET---------------------------------------

std::map<std::string, std::string> Response::_errorCode;

void  Response::initErrorCode(void)
{
	_errorCode["100"]  = "Continue";
	_errorCode["101"]  = "Switching Protocols";
	_errorCode["200"]  = "OK";
	_errorCode["201"]  = "Created";
	_errorCode["202"]  = "Accepted";
	_errorCode["203"]  = "Non-Authoritative Information";
	_errorCode["204"]  = "No Content";
	_errorCode["205"]  = "Reset Content";
	_errorCode["206"]  = "Partial Content";
	_errorCode["300"]  = "Multiple Choices";
	_errorCode["301"]  = "Moved Permanently";
	_errorCode["302"]  = "Found";
	_errorCode["303"]  = "See Other";
	_errorCode["304"]  = "Not Modified";
	_errorCode["305"]  = "Use Proxy";
	_errorCode["306"]  = "(Unused)";
	_errorCode["307"]  = "Temporary Redirect";
	_errorCode["400"]  = "Bad Request";
	_errorCode["401"]  = "Unauthorized";
	_errorCode["402"]  = "Payment Required";
	_errorCode["403"]  = "Forbidden";
	_errorCode["404"]  = "Not Found";
	_errorCode["405"]  = "Method Not Allowed";
	_errorCode["406"]  = "Not Acceptable";
	_errorCode["407"]  = "Proxy Authentication Required";
	_errorCode["408"]  = "Request Timeout";
	_errorCode["409"]  = "Conflict";
	_errorCode["410"]  = "Gone";
	_errorCode["411"]  = "Length Required";
	_errorCode["412"]  = "Precondition Failed";
	_errorCode["413"]  = "Request Entity Too Large";
	_errorCode["414"]  = "Request-URI Too Long";
	_errorCode["415"]  = "Unsupported Media Type";
	_errorCode["416"]  = "Requested Range Not Satisfiable";
	_errorCode["417"]  = "Expectation Failed";
	_errorCode["500"]  = "Internal Server Error";
	_errorCode["501"]  = "Not Implemented";
	_errorCode["502"]  = "Bad Gateway";
	_errorCode["503"]  = "Service Unavailable";
	_errorCode["504"]  = "Gateway Timeout";
	_errorCode["505"]  = "HTTP Version Not Supported";
}

std::string	Response::getReasonPhrase(std::string code)
{
	std::map<std::string, std::string>::iterator  it;


	it = _errorCode.find(code);
	return (it->second);
}

std::string	Response::getReasonPhrase(int code)
{
	std::stringstream ss;
	std::string nbr;
	std::map<std::string, std::string>::iterator  it;

	ss << code;
	nbr = ss.str();
	it = _errorCode.find(nbr);
	return (it->second);
}

std::string	Response::getErrorPage(int code)
{
       std::stringstream       ss;
       std::string             Page;

       ss << "<html><head><title>" << code <<" "<< getReasonPhrase(code) <<"</title></head><body>"
               <<"<center><h1>" << code <<" " << getReasonPhrase(code) <<"</h1></center> "
               << "<hr><center>poheck/1.0.0 (genereted)</center></body></html>";
       Page = ss.str();
       return (Page);
 }


Response::~Response()
{
}
