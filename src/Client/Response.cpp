#include "Response.hpp"

//-------------------------------------------------Constructor---------------------------------------

Response::Response()
{
	initErrorCode();
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

//-------------------------------------------------File---------------------------------------

void Response::OpenResponseFile(const char *path)
{
	std::stringstream	ss;
	// char 				buf[BUFFSIZE + 1] = {0};
	std::ifstream		file(path, std::ifstream::in);

	if (file.is_open())
	{	
		ss << file.rdbuf();
		_body = ss.str();
		file.close();
	}
	else
		_body = getErrorPage(403); 	
}

void    Response::generate()
{
	if (_request.badCode(_request.getCode()))
		invalidClient();
	else if (_request.getMethod() == "GET")
		methodGet();
	// else if (_request.getMethod() == "POST")
	// 	methodPost();
	// else
	// 	methodDelete();	
}

//-------------------------------------------------HEADER/BODY---------------------------------------

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

std::string		Response::getContentType(void)
{
	std::string	path = _request.getFullUri();
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

void	Response::invalidClient(void)
{
	std::stringstream ss;
	std::string tmp;

	//body
	generateBody();
	//Header
	generateHeader();
}

void	Response::generateBody(void)
{
	if (!_request.badCode(_code) && _request.isDir(_request.getFullUri()) == 0)
		_body = Autoindex::getPage(_request.getURI(), _request.getFullUri(), _request.getHost());
	else if (!_request.badCode(_code) && _request.isFile(_request.getFullUri()) == 0)
		OpenResponseFile(_request.getFullUri().c_str());
	else if (_request.isFile(_request.getFullUri()) == -1)
		_body = getErrorPage(404);
	else
		_body = getErrorPage(_code);

}

void	Response::generateHeader(void)
{
	std::stringstream ss;
	std::string tmp;

	ss << "HTTP/1.1" << " " << _request.getCode() << " " << getReasonPhrase(_request.getCode()) << "\r\n";
	ss << "Content-Type: " << getContentType() << "\r\n";
	ss << "Content-Length: " << _body.size() << "\r\n";
	ss << "Server: poheck\r\n";
	if (_request.getConnection() == "keep-alive")
		ss << "Keep-Alive: timeout=" << _request.getLifeTime() << "\r\n";
	ss << "Date: " << getTime() << "\r\n";
	ss << "\r\n";
	_header = ss.str();
}

void	Response::methodGet(void)
{

	generateBody();
	generateHeader();
	std::cout << GREEN << "GET method called\n" << ZERO_C;

	
	
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
               << "<hr><center>poheck/1.0.0 (KDE)</center></body></html>";
       Page = ss.str();
       return (Page);
 }


Response::~Response()
{
}
