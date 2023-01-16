/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:38:18 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 15:09:31 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/webserv.hpp"
#include "../include/Request.hpp"
#include "../include/errors.hpp"
#include "../include/headers.hpp"

void WebServ::evUpdate(int kqfd, int fd, int filter, int flags)
{
    struct kevent ev[2];
    int n = 0;
    EV_SET(&ev[n++], fd, filter, flags, 0, 0, NULL);
    if (kevent(kqfd, ev, n, NULL, 0, NULL) < 0)
		WebServ::err("kevent failure");
}

void WebServ::acceptClient(int kqfd, int fd)
{
    struct sockaddr_in remoteAddr;
    socklen_t rsz = sizeof(remoteAddr);

    int cfd = accept(fd,(struct sockaddr *)&remoteAddr,&rsz);
    if (cfd < 0)
		WebServ::err("accept():");
	std::string clientIP = std::to_string(int(remoteAddr.sin_addr.s_addr&0xFF))\
		+ std::string(".") + std::to_string(int((remoteAddr.sin_addr.s_addr&0xFF00)\
		>>8)) + std::string(".") + std::to_string(int((remoteAddr.sin_addr.s_addr&0xFF0000)\
		>>16)) + std::string(".") + std::to_string(int((remoteAddr.sin_addr.s_addr&0xFF000000)>>24));
    std::cout << "New connection from: \033[31;3m" << clientIP << "\033[0m on port: \033[32;3m" <<\
		ntohs(remoteAddr.sin_port) << "\033[0m !" << std::endl;
	if (fcntl(cfd, F_SETFL, O_NONBLOCK) < 0)
		WebServ::err("fcntl()");
    evUpdate(kqfd, cfd, EVFILT_READ, EV_CLEAR|EV_ADD|EV_ENABLE);
}

int WebServ::ifFinish(std::vector<char> &vec)
{
	std::size_t sizeHeadr = 0, t = 0, l = 0;
	std::string contentLen = "Content-Length:";
	std::string transferEnc = "Transfer-Encoding:";
	std::string tmpString;
	u_long length = 0;
	std::vector<char>::iterator it = vec.begin();
	std::vector<char>::iterator ite = vec.end();
	std::vector<char>::iterator tmp;
	bool err = false;
	int count  = 0;
	while (it != ite)
	{
		l = 0;
		t = 0;
		tmp = it;
		while (true)
			if (*tmp == contentLen[l]){++tmp;l++;}
			else if (*tmp == transferEnc[t]){++tmp;t++;} else
				break;
		if (l == contentLen.length())
		{
			count++;
			while (*tmp != '\r' and *(tmp + 1) != '\n')
			{
				tmpString.append(1, *tmp);
				++tmp;
			}
			try
			{
				length = std::stoul(tmpString, nullptr, 10);
			}
			catch(...)
			{
				;
			}

		}
		else if (t == transferEnc.length())
		{
			count++;
			while (*tmp != '\r' and *(tmp + 1) != '\n')
			{
				tmpString.append(1, *tmp);
				++tmp;
			}
		}
		if (*it == '\r' and *(it + 1) == '\n' and *(it + 2) == '\r' and *(it + 3) == '\n')
		{
			sizeHeadr = std::distance(vec.begin(), it) + 4;
			break;
		}
		++it;
	}
	if (err == true)
		return (1);
	if (*it == '\r' and *(it + 1) == '\n' and *(it + 2) == '\r' and *(it + 3) == '\n')
	{
		if ((tmpString != " chunked" and length == 0) or tmpString.empty())
			return (1);
		if (count > 1 )
			return (1);
		if (tmpString == " chunked")
		{
			if (*(vec.end() - 1) == '\n' and *(vec.end() - 2) == '\r' and *(vec.end() - 3)\
				== '\n' and *(vec.end() - 4) == '\r' and *(vec.end() - 5) == '0')
				return (1);
		}
		else
			if (sizeHeadr + length == vec.size())
				return (1);
	}
	return (0);
}

void WebServ::recvData(int fd, u_long length, int kqfd)
{
	port.clear();
	s_client t_client;
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(fd, (struct sockaddr *)&sin, &len) == -1)
		perror("getsockname");
	else
		port = std::to_string(ntohs(sin.sin_port));
	char buffer[length];
	int n = recv(fd, buffer, length, 0);
	if (n == 0)
	{
		std::cout << "Disconnected !" << std::endl;
		evUpdate(kqfd, fd, EVFILT_READ, EV_DELETE);
		close(fd);
	}
	if (n == -1)
		return;
	t_client.data.insert(t_client.data.end(), buffer, buffer + length);
	t_client.start = false;
	std::map<int, s_client>::iterator it = client.find(fd);
	if (it != client.end())
    	it->second.data.insert(it->second.data.end(), t_client.data.begin(), t_client.data.end());
	else
		client.insert(std::make_pair<int, s_client>(fd, t_client));
	n = ifFinish(client.at(fd).data);
	if (n)
	{
		evUpdate(kqfd, fd, EVFILT_WRITE, EV_CLEAR|EV_ENABLE | EV_ADD);
		evUpdate(kqfd, fd, EVFILT_READ, EV_DISABLE);
	}
	else
		client[fd].start = true;
	client[fd].stime = std::time(NULL);
}

std::string WebServ::buildResponse()
{
	std::string response;
	mime.clear();
	std::string method = req.getMethod();
	int status = req.CheckPath();
	int server = req.getServer(port);
	if (!tmpConfig[server].server_name.empty())
	{
		std::string tmp(req.getHostname());
		std::string host;
		std::string port;
		int i = -1, j = 0;
		std::size_t pos = tmp.find_last_of(':');
		if (pos != std::string::npos)
		{
			host = tmp.substr(0, pos );
			port = tmp.substr(pos + 1);
		}
		while (static_cast<std::size_t>(++i) < tmpConfig[server].server_name.size())
		{
			if (host == tmpConfig[server].server_name[i])
				if (port == tmpConfig[server].listen.port)
					j = 1;
			std::cout <<  tmpConfig[server].server_name[i] << std::endl;
		}
		if (j == 0)
		{
			response = header(404), w = 404;
			return (response);
		}
	}
	int ret;
	if ((ret = req.CheckStatusLine()))
	{
		response = header(ret);
		w = ret;
		return (response);
	}
	int error = req.getError();
	if (error)
	{
		switch (error)
		{
			case CONTENTLENERR:
				response =  header(400), w = 400;
				break;

			case ENTITYTOOLARGE:
				response =  header(413), w = 413;
				break;

			case LENREQUIRED:
				response =  header(411), w = 411;
				break;

			case COOKIETOOLARGE:
				response =  header(494), w = 494;
				break;

			default:
				response =  header(400), w = 400;
		}
	}
	else if (method == "GET")
	{
		mime = req.getMimeType(req.getPath());
		std::string cgiHeadersTmp;
		switch (status)
		{
			case -120:
				w = -120;
				response = header(202);
				break;

			case 200:
				w = 200;
				response = header(200);
				break;

			case 301:
				w = 301;
				response = header(301);
				break;

			case -200:
				if (req.getRespchunked() == true)
					response = header(1000), w = 1000;
				else
					response = header(-200), w = -200;
				break;

			case -301:
				w = -301;
				cgiHeadersTmp = req.getHeaderString();
				response = header_301 + cgiHeadersTmp + CRLF2;
				break;

			case -302:
				w = -302;
				cgiHeadersTmp = req.getHeaderString();
				response = header_302 + cgiHeadersTmp + CRLF2;
				break;

			case -307:
				w = -307;
				cgiHeadersTmp = req.getHeaderString();
				response = header_307 + cgiHeadersTmp + CRLF2;
				break;

			default:
				w = status;
				response = header(status);
		}
	}
	else if (method == "POST")
	{
		std::string cgiHeadersTmp;
		switch (status)
		{
			case -120:
				w = -120;
				response = header(202);
				break;

			case 200:
				w = 200;
				response = header(200);
				break;

			case 202:
				w = 202;
				response = header(202);
				break;

			case 301:
				w = 301;
				response = header(301);
				break;

			case -200:
				if (req.getRespchunked() == true)
					response = header(1000),w = 1000;
				else
					response = header(-200), w = -200;
				break;

			case -301:
				w = -301;
				cgiHeadersTmp = req.getHeaderString();
				response = header_301 + cgiHeadersTmp + CRLF2;
				break;

			case -302:
				w = -302;
				cgiHeadersTmp = req.getHeaderString();
				response = header_302 + cgiHeadersTmp + CRLF2;
				break;

			case -307:
				w = -307;
				cgiHeadersTmp = req.getHeaderString();
				response = header_307 + cgiHeadersTmp + CRLF2;
				break;

			default:
				w = req.CheckPath();
				response = header(w);
		}
	}
	else if (method == "DELETE")
	{

		if (std::remove(req.getFull_path().c_str()) == 0)
			response = header_204, w = 204;
		else
			response = header_404 + " 0\r\n\r\n", w = 404;
	}
	else
		w = req.CheckStatusLine(), response = header(w);
	return (response);
}

int WebServ::checkMethod()
{
	std::string method = req.getMethod();
	int server = req.getServer(port);
	std::string tmp = tmpConfig[server].allowed_methods;
	std::vector<std::string> tmpv = lexer::cppSplit(tmp, ',');
	std::vector<std::string>::iterator it = std::find(tmpv.begin(), tmpv.end(), method);
	if (it == tmpv.end())
		return (405);
	return(0);
}

std::vector<std::string> WebServ::getChunkedResponse()
{
	std::vector<std::string> response;
	std::string tmpbuffer = req.getContent();
	response.push_back(header(1000));
	for (std::size_t i = 0; i < tmpbuffer.length(); i += CHUNKSIZE)
	{
		std::size_t chunksize;
		if (i + 128 > tmpbuffer.length())
			chunksize = tmpbuffer.length() - i;
		else
			chunksize = tmpbuffer.substr(i, CHUNKSIZE).length();
		std::stringstream ss;
		ss << std::hex << chunksize;
		std::string result(ss.str());
		response.push_back(result + CRLF + tmpbuffer.substr(i, CHUNKSIZE) + CRLF);
	}
	response.push_back(std::string("0") + CRLF2);
	return (response);
}

WebServ::WebServ(std::vector<s_server> config): Server(config) , tmpConfig(config)
{
	int kqFd = kqueue();
	if (kqFd < 0)
		WebServ::err("kq failed");
	Socket *tmp = getLSock();
	int *tmp1 = tmp->getSockets();
	for (int i = 0; i < tmp->size; i++)
		evUpdate(kqFd, tmp1[i], EVFILT_READ,EV_ADD|EV_ENABLE);
	while (true)
	{
		for (int i = 0 ; i < tmp->size; i++)
		{
			struct timespec timeout;
			timeout.tv_sec = TIMEOUT / 1000;
			timeout.tv_nsec = (TIMEOUT % 1000) * 1000 * 1000;
			struct kevent eventsList[KMAXEVENTS];
			int n = kevent(kqFd, NULL, 0, eventsList, KMAXEVENTS, &timeout);
			for (int j = 0; j < n; j++)
			{
				int fd = static_cast<int>(eventsList[j].ident);
				int events = eventsList[j].filter;
				if (eventsList[j].flags & EV_EOF)
				{
					evUpdate(kqFd, fd, EVFILT_READ , EV_CLEAR | EV_DELETE);
					std::cout << "Disconnected !" << std::endl;
					close(fd);
				}
				else if (fd == tmp1[i])
					acceptClient(kqFd, fd);
				else if (events == EVFILT_READ)
					recvData(fd, eventsList[j].data, kqFd);
				else if (events == EVFILT_WRITE)
				{
					sendData(fd);
					evUpdate(kqFd, fd, EVFILT_WRITE, EV_DISABLE);
					evUpdate(kqFd, fd, EVFILT_READ, EV_CLEAR | EV_ENABLE | EV_ADD);
					client.erase(fd);
				}
			}
			std::map<int, s_client>::iterator it = client.begin();
			for (; it != client.end(); ++it)
			{
				if (it->second.start)
				{
					if ((std::time(NULL) - it->second.stime) >= 10)
					{
						req.setError(TIMEDOUT);
						it->second.start = false;
						evUpdate(kqFd, it->first, EVFILT_WRITE, EV_CLEAR | EV_ENABLE | EV_ADD);
						evUpdate(kqFd, it->first, EVFILT_READ, EV_DISABLE);
						client.erase(it->first);
						break;
					}
				}
			}
		}
	}
}

std::string WebServ::error_page(int error)
{
	int server = req.getServer(port);
	int i = 0;
	int j = tmpConfig[server].errvec.size();

	while (i < j)
	{
		if (tmpConfig[server].errvec[i].error == error)
			return (tmpConfig[server].errvec[i].content);
		i++;
	}
	switch (error)
	{
		case 400:
			return (webserv_400_page);

		case 403:
			return (webserv_403_page);

		case 404:
			return (webserv_404_page);

		case 405:
			return (webserv_405_page);

		case 408:
			return (webserv_408_page);

		case 411:
			return (webserv_411_page);

		case 413:
			return (webserv_413_page);

		case 494:
			return (webserv_494_page);

		case 500:
			return (webserv_500_page);

		case 501:
			return (webserv_501_page);

		case 505:
			return (webserv_505_page);

		default:
			break;
	}
	return (NULL);
}

std::string WebServ::header(int error)
{
	struct timeval tv;
	std::time_t t;
	std::tm *info;
	std::string ret;
	gettimeofday(&tv, NULL);
	t = tv.tv_sec;
	info = std::localtime(&t);
	std::string date = std::asctime(info);
	date.erase(date.find_last_of(LF));
	std::string cgiheaders;

	switch (error)
	{
		case 1000:
			w = 1000;
			cgiheaders = req.getHeaderString();
			ret = "HTTP/1.1 200 OK\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nTransfer-Encoding: chunked\r\nConnection: Keep-Alive\r\n" + cgiheaders + CRLF;
			return (ret);

		case -200:
			w = -200;
			cgiheaders = req.getHeaderString();
			ret = "HTTP/1.1 200 OK\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\n" + cgiheaders + CRLF2;
			return (ret);

		case 200:
			w = 200;
			ret = "HTTP/1.1 200 OK\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(req.getBodyvec().size()) + CRLF + req.getMimeType(req.getPath()) + CRLF2;
			return (ret);

		case 202:
			w = 202;
			ret = "HTTP/1.1 200 OK\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(req.getContent().length()) + CRLF + req.getMimeType() + CRLF2;
			return (ret);

		case 301:
			w = 301;
			ret = "HTTP/1.1 301 Moved Permanently\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nLocation: " \
				+ req.getNewlocation() + CRLF2;
			return (ret);

		case 400:
			w = 400;
			ret = "HTTP/1.1 400 Bad Request\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(400).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(400);
			return (ret);

		case 403:
			w = 403;
			ret = "HTTP/1.1 403 Forbidden\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(403).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(403);
			return (ret);

		case 404:
			w = 404;
			ret = "HTTP/1.1 404 Not Found\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(404).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(404);
			return (ret);

		case 405:
			w = 405;
			ret = "HTTP/1.1 405 Method Not Allowed\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(405).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(405);
			return (ret);

		case 408:
			w = 408;
			ret = "HTTP/1.1 408 Request Timeout\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(408).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(408);
			return (ret);

		case 413:
			w = 413;
			ret = "HTTP/1.1 413 Request Entity Too Large\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(413).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(413);
			return (ret);

		case 411:
			w = 411;
			ret = "HTTP/1.1 411 Length Required\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(411).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(411);
			return (ret);

		case 494:
			w = 494;
			ret = "HTTP/1.1 400 Bad Request\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(494).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(494);
			return (ret);

		case 501:
			w = 501;
			ret = "HTTP/1.1 501 Not Implemented Too Large\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(501).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(501);
			return (ret);

		case 505:
			w = 505;
			ret = "HTTP/1.1 505 HTTP Version Not Supported\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(505).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(505);
			return (ret);

		case 500:
			w = 500;
			ret = "HTTP/1.1 500 Internal Server Error\r\nDate: " + date + CRLF + "Server: Webserv (Mac OS)\r\nContent-Length: " \
				+ std::to_string(error_page(500).length()) + CRLF + req.getMimeType() + CRLF2 + error_page(500);
			return (ret);

		default:
			break;
	}
	return (NULL);
}


void WebServ::sendData(int fd)
{
	std::string r;
	if (this->req.getError() == TIMEDOUT)
	{
		this->req.setError(0);
		r = header(408) + webserv_408_page;
		w = 408;
	}
	else
	{
		this->req = Request(client[fd].data, tmpConfig, port, fd);
		r = buildResponse();
	}
	std::vector<char *> response;
	std::vector<std::string> chunked;
	std::size_t *sizes = {nullptr};
	std::size_t iterations = 0;
	std::string tmp = req.getContent();
	switch (w)
	{
		case 200:
			sizes = new std::size_t[2];
			sizes[0] = r.size();
			sizes[1] = req.getBodyvec().size();
			response.push_back(&r[0]);
			response.push_back(req.getBodyvec().data());
			iterations = 2;
			break;

		case -120: case -200: case 202:
			sizes = new std::size_t[2];
			sizes[0] = r.size();
			sizes[1] = tmp.size();
			response.push_back(&r[0]);
			response.push_back(&tmp[0]);
			iterations = 2;
			break;

		case -301: case -302: case -307: case 203 ... 505:
			sizes = new std::size_t[1];
			sizes[0] = r.size();
			sizes[1] = 0;
			response.push_back(&r[0]);
			iterations = 1;
			break;

		case 1000:
			chunked = getChunkedResponse();
			sizes = new std::size_t[chunked.size()];
			for (std::size_t i = 0; i < chunked.size(); i++)
			{
				sizes[i] = chunked[i].size();
				response.push_back(&chunked[i][0]);
			}
			iterations = chunked.size();
			response.push_back(nullptr);

		default:
			break;
	}
	for (std::size_t i = 0; i < iterations; i++)
	{
		if (response[i] == NULL or sizes[i] == 0)
			break;
		int n = 0;
		while (sizes[i] != 0)
		{
			if ((n = send(fd, response[i], sizes[i], 0)) < 0)
				continue;
			sizes[i] -= n;
    		response[i] += n;
		}
	}
	w = 0;
	if (sizes)
		delete[] sizes;
}

std::vector<std::string> WebServ::autoindex(std::string Path)
{
	struct dirent *entry;
	DIR *dir;
	std::vector<std::string> content;
	int i = 0;
	if ((dir = opendir(Path.c_str())) == NULL)
		return (content);
	while ((entry = readdir(dir)) != NULL)
	{
		i++;
		if (i > 2) content.push_back(std::string(entry->d_name));
	}
	closedir(dir);
	return (content);
}

void WebServ::err(const char *str)
{
	std::cout << "Error:\n" << str << std::endl;
}


WebServ::~WebServ()
{
}
