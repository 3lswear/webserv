#include "CgiHandle.hpp"

CgiHandle::CgiHandle(Request &request, Response &response):
	_request(request), _response(response)
{
	initEnvVariables();
}

std::string toString(unsigned long var)
{
	std::stringstream ss;
	ss << var;

	return (ss.str());
}

std::string getEnvFormat(std:: string tmp)
{
	std::string str = tmp;
	std::replace(str.begin(), str.end(), '-', '_');
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return (str);
}

char    **CgiHandle::mapToCharPtr(void)
{
	char **tmp = new char *[_variable.size() + 1];
	std::map<std::string, std::string>::iterator    it;

	int i = 0;
	std::string value;
	for (it = _variable.begin(); it != _variable.end(); it++)
	{
		if (it->second.empty())
			continue;
		value = it->first + "=" + it->second;
		tmp[i] = new char[value.size() + 1];
		strcpy(tmp[i], value.data());
		i++;
	}
	tmp[i] = NULL;

	return (tmp);    
}

void	CgiHandle::freeEnvVariables()
{
	for (size_t i = 0; env[i]; i++)
	{
		delete[] env[i];
	}
	delete[] env;
	
}

void	CgiHandle::printSSmap(std::map<std::string, std::string> &m)
{
	std::map<std::string, std::string>::iterator it;

	DBOUT << WARNING << getDebugTime() << FAIL << "Print MAP" << ENDL;

	it = m.begin();
	for (; it != m.end(); it++)
	{
		DBOUT << BLUE << it->first << PINK << it->second << ENDL;
	}
}

void	printenv(char **env)
{
	DBOUT << WARNING << getDebugTime() << FAIL << "Print env" << ENDL;
	for(size_t i = 0; env[i]; i++)
	{
		DBOUT << RED << env[i] << ENDL;
	}
}

std::string CgiHandle::executeCgi()
{
	char		*argv[2];
	pid_t		pid;
	int			sI;
	int			sO;
	int			byte_read = 1;
	std::string body;

	argv[0] = const_cast<char *>(_response._fullURI.data());
	argv[1] = NULL;
	try
	{
		env = mapToCharPtr();
	}
	catch(const std::bad_alloc& e)
	{
		std::cerr << WARNING << getDebugTime()  << FAIL <<  e.what()  << ENDL;
	}
	sI = dup(STDIN_FILENO);
	sO = dup(STDOUT_FILENO);

	FILE *fIn = tmpfile();
	FILE *fOt = tmpfile();
	long	fdin = fileno(fIn);
	long	fdOut = fileno(fOt);
	if (_request.getBody() != NULL)
		write(fdin, _request.getBody()->data(), _request.getBody()->size());

	lseek(fdin, 0, SEEK_SET);
	pid = fork();
	if (pid == -1)
	{
		std::cerr << WARNING << getDebugTime() <<  FAIL << "Pid = -1. Fork error."<< ENDL;
	}
	else if (pid == 0)
	{
		dup2(fdin, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(_response.getCgiPass().c_str(), argv, env);
		std::cerr << getDebugTime() <<  FAIL;
		perror("Execve error");
		std::cerr << RESET;
		write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15); 	
		exit(1);
	}
	else
	{
		char buffer[BUFFSIZE + 1] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		byte_read = 1;
		while (byte_read)
		{
			memset(buffer, 0, BUFFSIZE + 1);
			byte_read = read(fdOut, buffer, BUFFSIZE);
			body.append(buffer, byte_read);
		}
	}
	dup2(sI, STDIN_FILENO);
	dup2(sO, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOt);
	close(fdin);
	close(fdOut);
	close(sI);
	close(sO);
	freeEnvVariables();	
	return (body);
}

void    CgiHandle::initEnvVariables()
{
	std::map<std::string, std::string>::iterator    it;
	std::map<std::string, std::string> tmp1 =  _request.getClientFields();
	_scriptName = _response._fullURI.substr(_response._fullURI.rfind("/"));
	it = tmp1.find("content_type");
	if (it != tmp1.end())
		_variable["AUTH TYPE"] = it->second;
	else
		_variable["AUTH TYPE"] = "";
	_variable["CONTENT_LENGTH"] = (_request.getBody() == NULL) ? "0" : toString(_request.getBody()->size());
	it = _request.getClientFields().find("content-type");
	_variable["CONTENT_TYPE"] = "";
	_variable["GATEWAY_INTERFACE"] = std::string("CGI/1.1");

	_variable["SCRIPT_NAME"] = _response._fullURI;
	_variable["SCRIPT_FILENAME"] = _response._fullURI;

	_variable["PATH_TRANSLATED"] = _request.getURI();
	_variable["REQUEST_URI"] = _request.getURI();
	_variable["PATH_INFO"] = _request.getURI();
	
	_variable["QUERY_STRING"] = _request.getQuery();
	_variable["REMOTE_ADDR"] = _response.getListen().ip;
	_variable["REMOTE_HOST"] = _response.getListen().ip;
	_variable["REMOTE_IDENT"] = "";
	_variable["REMOTE_USER"] = "";
	_variable["REQUEST_METHOD"] = _request.getMethod();
	_variable["SERVER_NAME"] = _request.getHost();
	_variable["SERVER_PORT"] = toString(_response.getListen().port);
	_variable["SERVER_PROTOCOL"] = "HTTP/1.1";
	_variable["REDIRECT_STATUS"] = "200";
	_variable["SERVER_SOFTVARE"] = "poheck/1.0.0";

	it = tmp1.begin();
	std::string tmp;
	for (; it != tmp1.end(); it++)
	{
		tmp = getEnvFormat(it->first);
		_variable["HTTP_" + tmp] = it->second;
	}
	// printSSmap(_variable);
}

CgiHandle::~CgiHandle()
{
}
