/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIx.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:09 by ahkecha           #+#    #+#             */
/*   Updated: 2023/01/15 18:38:42 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/CGIx.hpp"
#include <sstream>
#define CRLF "\r\n"

CGI::CGI(s_server &Server, s_location &location ,std::string &method,\
		std::string &path, std::string &fquery, std::string &body,\
		std::string &type, char *rawBody, std::string &pathInfo,\
		std::string &cgiExtension, std::map<std::string,\
		std::string> &cookie, std::string &full_path)
{
	contentLen = true;
	this->method = method;
	this->path = path;
	this->query = fquery;
	this->host = Server.listen.host;
	this->port  = Server.listen.port;
	if (location.root.empty())
		this->root = Server.root;
	else
	{
		this->root = location.root;
	}
	this->full_path = full_path;
	this->cgiPath = location.cgiPath;
	this->type = type;
	this->body = body;
	this->rawbody = rawBody;
	this->pathinfo = pathInfo;
	this->cgiextension = cgiExtension;

	for (std::map<std::string, std::string>::iterator it = cookie.begin(); it != cookie.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		if (std::distance(it, cookie.end()) == 1)
			;
		else
			tmp.append("; ");
		httpcookies.append(tmp);
	}
	this->constructEnv();
	this->executeCGI();
}

void CGI::constructEnv()
{
	if (method == "POST")
	{
		vec.push_back(std::string("CONTENT_LENGTH=" + std::to_string(body.length() - 1)));
		vec.push_back(std::string("BODY=" + body));
		vec.push_back(std::string("CONTENT_TYPE=" + type));
	}

	vec.push_back(std::string("QUERY_STRING=" + query));
	vec.push_back(std::string("SCRIPT_FILENAME=" + root + path));
	vec.push_back(std::string("REQUEST_METHOD=" + method));
	vec.push_back(std::string("SERVER_PORT=" + port));
	vec.push_back(std::string("REDIRECT_STATUS=200"));
	vec.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	vec.push_back(std::string("PATH_INFO=" + pathinfo));
	vec.push_back(std::string("HTTP_COOKIE=" + httpcookies));
}

void CGI::executeCGI()
{
	char *env[this->vec.size() + 1];

	int fds[2];
	if (pipe(fds) < 0)
		perror("pipe():");
	for (std::size_t i = 0 ; i < vec.size(); i++)
		env[i] = const_cast<char *>(vec[i].c_str());
	env[this->vec.size()] = NULL;

	FILE *fp = std::tmpfile();
	int fd = ::fileno(fp);


	char *args[3] = {
		&cgiPath[0],
		&full_path[0],
		NULL
	};
	std::fwrite(body.c_str(), sizeof(body), body.size(), fp);
	std::rewind(fp);

	int status;
	error = false;

	pid_t pid = fork();
	if(pid == 0)
	{
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		dup2(fds[1], STDERR_FILENO);
		dup2(fd, STDIN_FILENO);
		execve(*args, args, env);
		perror("execve: ");
		exit(99);
	}
	wait(&status);
	close(fds[1]);
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) != 0)
		{
			if (WEXITSTATUS(status) == 1 and cgiextension == "py")
				;
			else
			{
				error = true;
				return;
			}
		}
	}
	stringbuff.clear();
	while(true)
	{
		char buff[2] = {0};
		if (read(fds[0], buff, 1) <= 0)
			break;
		stringbuff.append(buff);
	}
	close(fds[0]);
}

std::vector<std::string> CGI::getCGIHeaders()
{
	std::stringstream sStream;
	std::string tmp;
	std::vector<std::string> tmpVec;
	sStream << stringbuff;
	while (std::getline(sStream, tmp) and (tmp[0] != CR and tmp.size() != 0))
	{
		if (tmp.find_last_of('\r') != std::string::npos)
			tmp.erase(tmp.find_last_of('\r'));
		if (tmp.find("Content-Length:") == std::string::npos)
			contentLen = false;
		if (tmp.find("Status:") != std::string::npos)
			continue;
		tmpVec.push_back(tmp);
	}
	return (tmpVec);
}

std::string CGI::getContent()
{
	std::stringstream sStream;
	std::string tmp;
	std::string ret;
	sStream << stringbuff;
	while (std::getline(sStream, tmp) and (tmp[0] != CR and tmp.size() != 0))
		;
	tmp.clear();
	while (std::getline(sStream, tmp))
		ret += tmp;
	return (ret);
}



int CGI::getStatusCode()
{
	std::size_t pos = stringbuff.find("Status:");
	if (pos != std::string::npos)
	{
		int tmp = std::stod(stringbuff.substr(8, 3));
		if (tmp >= 301)
			return (tmp);
	}
	return (0);
}



std::string &CGI::getBody()
{
	return (this->body);
}

CGI::~CGI()
{

}
