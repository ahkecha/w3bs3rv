/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:03 by ahkecha           #+#    #+#             */
/*   Updated: 2023/02/27 14:01:18 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Request.hpp"
#include "../include/webserv.hpp"
#include "../include/CGIx.hpp"
#include "../include/MimeTypes.hpp"

void Request::getUrlParams(std::string &str)
{
	fullQuery.clear();
	std::size_t pos = str.find('?');
	if (pos != std::string::npos)
	{
		fullQuery.append(str.substr(pos + 1));
		std::vector<std::string> splitted = lexer::cppSplit(fullQuery, '&');
		std::vector<std::string>::iterator it = splitted.begin();
		for (; it != splitted.end(); ++it)
		{
			struct urlParams tmp;
			std::size_t pos1 = it->find('=');
			if (pos1 != std::string::npos)
			{
				tmp.param = it->substr(0, pos1);
				tmp.value = it->substr(pos1 + 1);
				params.push_back(tmp);
			}
		}
		path = path.substr(0, pos);
	}
}

short checkMimeType(std::string &type)
{
	std::size_t pos = type.find(";");
	std::string tmp;
	if (pos != std::string::npos)
		tmp = type.substr(0, pos);
	else
		tmp = type;
	for (int i = 0; i < mimeTypes->size(); i++)
	{
		if (tmp == mimeTypes[i].mimetype)
			return (1);
	}
	return (0);
}

std::string Request::getMimeType(std::string path)
{
	if (path == UNSPEC)
	{
		std::string ret = std::string("Content-type: text/html");
		return (ret);
	}
	std::string ret = "Content-type: text/html";
	std::size_t pos = path.find_last_of('.');
	if (pos != std::string::npos)
	{
		std::string ext = path.substr(pos + 1);
		for (int i = 0; i < mimeTypes->size(); i++)
		{
			if (ext == mimeTypes[i].extension)
			{
				ret = std::string("Content-type: ") + mimeTypes[i].mimetype;
				break;
			}
		}
	}
	return (ret);
}

std::string Request::getHeaderString()
{
	std::string tmp;
	for (std::vector<std::string>::iterator it = cgiHeaders.begin(); it != cgiHeaders.end(); ++it)
		tmp.append(*it + "\r\n");
	return (tmp);
}

Request::Request():idx(0)
{
	method = "";
	path = "";
	ver = "";
	buffer = "";
}

int Request::getServer(std::string &port) const
{
	int size = configVec.size();
	int i = 0;
	while (i < size)
	{
		if (configVec[i].listen.port == port)
			break;
		i++;
	}
	return (i);
}

bool Request::isChunked()
{
	for (std::vector<header>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		if (it->headers == "Tranfer-Encoding" and it->value == "chunked")
		{
			return (true);
		}
	}
	return (false);
}

int Request::parseCookie(std::string &rawCookie)
{
	cookie.clear();
	if (rawCookie.size() > 4096)
		return(400);
	std::vector<std::string> tmpvec = lexer::cppSplit(rawCookie, ';');
	if (tmpvec.size() > 0)
	{
		for (std::vector<std::string>::iterator it = tmpvec.begin(); it != tmpvec.end(); it++)
		{
			std::size_t pos = it->find_first_of('=');
			if (pos != std::string::npos)
			{
				std::string key = it->substr(0, pos);
				trimWhiteSpaces(key, false);
				std::string value = it->substr(pos + 1);
				trimWhiteSpaces(value, false);
				value = decodeUrl(value);
				if (key.length() + value.length() > 1024)
					return (400);
				cookie.insert(std::pair<std::string, std::string>(key, value));
			}
		}
	}
	return (0);
}

Request::Request(std::vector<char> &vec, std::vector<s_server> tmpConfig, std::string &port, int fd)
{
	int ccount = 0;
	int tecount = 0;
	std::size_t pos = 0;
	chunked = false;
	std::string tmpbuff = vec.data();
	for(; pos < vec.size(); pos++)
	{
		if (vec[pos] == CR and vec[pos + 1] == LF and vec[pos + 2] == CR and vec[pos + 3] == LF)
			break;
	}
	if (pos)
	{
		buffer = tmpbuff.substr(0, pos);
		tmpbuff.erase(0, pos + 4);
	}
	body = tmpbuff;
	error = 0;
	socketfd = fd;
	isBinary = false;
	configVec = tmpConfig;
	idx = getServer(port);
	std::istringstream reqs(buffer);
	std::string tmp;
	std::string tmp2;
	if (!chunked)
	{
		std::getline(reqs, tmp);
		std::vector<std::string> splitted = lexer::cppSplit(tmp, ' ');
		if (splitted.size() == 3)
		{
			method = splitted[0];
			path = decodeUrl(splitted[1]);
			getUrlParams(path);
			ver = splitted[2];
		}
		while (std::getline(reqs, tmp) and (tmp[0] != CR and tmp.size() != 0))
		{
			struct header tmp1 = header();
			std::string::size_type pos;
			if ((pos = tmp.find(":")) != std::string::npos)
			{
				tmp1.headers = tmp.substr(0, pos);
				tmp1.value = tmp.substr(pos + 2);
				if ((pos = tmp1.value.find_last_of('\r')) != std::string::npos)
					tmp1.value.erase(pos);
				if (tmp1.headers == "Content-Length")
				{
					ccount++;
					for (std::size_t j = 0; j < tmp1.value.length(); j++)
					{
						if (!std::isdigit(tmp1.value[j]))
						{
							this->error = CONTENTLENERR;
							return;
						}
					}
					try
					{
						contentlen = std::stoul(tmp1.value);
					}
					catch(std::invalid_argument const &e)
					{
						this->error = CONTENTLENERR;
						return;
					}

				}
				if (tmp1.headers == "Transfer-Encoding")
				{
					tecount++;
					if(tmp1.value == "chunked")
						chunked = true;
					else
						error = CONTENTLENERR;
				}
				if (tmp1.headers == "Content-Type")
				{
					postContentType = tmp1.value;
					if (!checkMimeType(postContentType))
					{
						this->error = 1;
						return;
					}

				}
				if (tmp1.headers == "Cookie")
				{
					if (parseCookie(tmp1.value) == 400)
					{
						this->error = COOKIETOOLARGE;
						return;
					}
				}
				else if (tmp1.headers == "Host")
				{
					hostname = tmp1.value;
					trimWhiteSpaces(hostname, false);
				}
				headers.push_back(tmp1);
			}
		}
	}
	if (ccount == 0 and method == "POST")
	{
		error = LENREQUIRED;
		return;
	}
	if (ccount > 1 or tecount >= 1)
	{
		error = CONTENTLENERR;
		return;
	}
	if ((ccount == 1 or tecount == 1) and method == "GET")
	{
		error = CONTENTLENERR;
		return;
	}
	if (contentlen > tmpConfig[idx].maxsize and method == "POST")
	{
		error = ENTITYTOOLARGE;
		return;
	}
	if (method == "POST" and !chunked)
	{
		if (path == "/upload.cgi")
		{
			if (configVec[idx].upload.empty())
				configVec[idx].upload = configVec[idx].root;
			uploadFile(vec, pos + 4);
			vec.clear();
		}
		else
			body = vec.data() + pos + 4;
	}
	else if (method == "POST" and chunked)
	{
		bool isFile;
		if (path == "/upload.cgi")
			isFile = true;
		bodyvec.clear();
		for (std::vector<char>::iterator it = vec.begin() + pos + 4 ; it != vec.end();)
		{
			std::string sizestr;
			while (*it != '\r' and *it + 1 != '\n')
			{
				sizestr.push_back(*it);
				if (!std::isdigit(*it) and (*it < 97 or *it > 102))
					error = 646;
				it++;
			}
			it += 2;
			u_long size;
			try
			{
				size = std::stoul(sizestr, nullptr, 16);
			}
			catch(...)
			{
				this->error = CONTENTLENERR;
				return;
			}

			std::string n;
			for (u_long i = 0; i < size; i++)
			{
				n.push_back(*it);
				bodyvec.push_back(*it);
				it++;
			}
			if (*it == '\r' and *(it + 1) == '\n' and *(it + 2) == '\r' and *(it + 3) == '\n')
				it += 4;
			else
				it += 2;
		}
		if (isFile)
		{
			if (configVec[idx].upload.empty())
				configVec[idx].upload = configVec[idx].root;
			uploadFile(bodyvec,0);
		}
		else
			body = bodyvec.data();
		bodyvec.shrink_to_fit();
	}
}

int Request::CheckStatusLine() const
{
	bool skip = false;
	std::string tmp = configVec[idx].allowed_methods;
	std::vector<std::string> tmpv = lexer::cppSplit(tmp, ',');
	std::vector<std::string>::iterator it = std::find(tmpv.begin(), tmpv.end(), method);
	if (it == tmpv.end())
	{
		std::vector<const s_location>::iterator ite = configVec[idx].location.end();
		for(std::vector<const s_location>::iterator it = configVec[idx].location.begin(); it != ite; ++it)
		{
			std::string dir = path.substr(0, it->Path.size());
			if (dir == it->Path)
				if (it->allowed_methods.find(method) != std::string::npos) skip = true;
		}
		if (!skip)
			return (405);
	}
	if ((method.compare("GET") != 0 and method.compare("POST") != 0 and method.compare\
	("DELETE") != 0 ) and (method.compare("PATCH") == 0 or method.compare("PUT") == 0))
		return (405);
	else if (method.compare("GET") != 0 and method.compare("POST") != 0 and method.compare\
	("DELETE") != 0  and method.compare("PATCH") != 0 and method.compare("PUT") != 0)
		return (501);
	if (this->ver != "HTTP/1.1\r")
		return (505);
	return (0);
}

int Request::CheckPath()
{
	bool mode;
	content.clear();
	std::string root = configVec[idx].root;
	path.erase(std::unique(path.begin(), path.end(), bSlashes()), path.end());
	full_path = root + path;
	std::string tmp;
	struct stat s;

	if (path.find("../") != std::string::npos) return (403);
	if (hostname.empty()) return (400);
	if (path == "/upload.cgi")
	{
		if (method == "POST")
		{
			content = "<html>"
			"<head><title>Upload Sucess</title></head>"
			"<style type=\"text/css\">"
			"@import url(https://fonts.googleapis.com/css?family=Merriweather);"
			"@import url(https://fonts.googleapis.com/css?family=Dancing+Script);"
			"h1 {"
			"   font-family: \"Merriweather\", sans-serif;"
			"   -webkit-font-smoothing: antialiased;"
			" 	text-rendering: geometricPrecision;"
			"}"
			".bottom {"
			"	font-family: \"Dancing Script\", sans-serif;"
			"	font-size: 1.5em;"
			"</style>"
			"<body>"
			"<center><h1>File uploaded successfully</h1></center>"
			"<hr><center class=\"bottom\">Webserv</center>"
			"</body>"
			"</html>"
			;
			return (-120);
		}
		else return (405);
	}
	for (std::size_t i = 0; i < configVec[idx].location.size(); i++)
	{
		std::string tmpPath = configVec[idx].location[i].Path;
		if ((path == tmpPath) or (path.substr(0,tmpPath.size() + 1) == std::string(tmpPath + "/")))
		{
			if (!configVec[idx].location[i].redirect.empty())
			{
				newlocation = configVec[idx].location[i].redirect;
				return (301);
			}
			if (configVec[idx].location[i].deny == "all")
				return (403);
			if (!configVec[idx].location[i].root.empty())
			{
				full_path = configVec[idx].location[i].root + path.substr(tmpPath.size());
				if (path == tmpPath)
				{
					if (!configVec[idx].location[i].index.empty())
					{
						path = configVec[idx].location[i].index;
						full_path = configVec[idx].location[i].root + "/" + path;
					}
				}
			}
			if (!configVec[idx].location[i].index.empty() and configVec[idx].location[i].root.empty())
			{
				full_path = configVec[idx].root + path + "/" + configVec[idx].location[i].index;
				mode = true;
			}
			else if (configVec[idx].location[i].autoindex == true and full_path != configVec[idx].location[i].Path)
			{
				if (stat(full_path.c_str(), &s) == 0)
				{
					if (s.st_mode & S_IFDIR)
					{
						std::vector<std::string> dirContent = WebServ::autoindex(full_path);
						constructResponse(dirContent, path);
						return (202);
					}
				}
			}
			std::size_t dot = path.find('.');
			if (dot != std::string::npos)
			{
				std::string tmpext = path.substr(dot + 1, 3);
				if (tmpext == configVec[idx].location[i].cgiExt)
				{
					tmpext = path.substr(dot + 1);
					std::string path_info;
					try
					{
						path_info = tmpext.substr(tmpext.find_first_of('/'));
					}
					catch(const std::exception& e)
					{
						path_info = path;
					}

					CGI test(configVec[idx],
							configVec[idx].location[i],
							method,
							path,
							fullQuery,
							body,
							postContentType,
							raw,
							path_info,
							tmpext,
							cookie,
							full_path);

					if (test.error == true)
						return (500);
					int status = test.getStatusCode();
					if (status > 307 and status <= 507)
						return (status);
					content = std::string(test.getContent().c_str());
					cgiHeaders = test.getCGIHeaders();
					for (std::size_t i = 0; i < cgiHeaders.size(); i++)
					{
						if (cgiHeaders[i].find("Location:") != std::string::npos)
						{
							switch (status)
							{
							case 301:
								return(-301);
								break;

							case 302:
								return (-302);
								break;

							case 307:
								return (-307);
								break;

							default:
								break;
							}
						}
						if (test.contentLen == false)
							respchunked = true;
					}
					return (-200);
				}
			}
		}
		tmpPath.clear();
	}
	if ((path == "/" and configVec[idx].autoindex == true and configVec[idx].index.empty()) \
		or (path != "/" and configVec[idx].autoindex == true and !mode))
	{
		if (stat(full_path.c_str(), &s) == 0)
		{
			if (s.st_mode & S_IFDIR)
			{
				std::vector<std::string> dirContent = WebServ::autoindex(full_path);
				constructResponse(dirContent, path);
				return (202);
			}
		}
	}
	if (!configVec[idx].index.empty() && path == "/")
	{
		full_path = root + path + configVec[idx].index;
		path = "/" + configVec[idx].index;
	}
	std::ifstream file(full_path, std::ios::binary);
	if (file.fail())
		return (404);
    file.unsetf(std::ios::skipws);
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
	bodyvec.clear();
    bodyvec.reserve(fileSize);
    bodyvec.insert(bodyvec.begin(),
        std::istream_iterator<char>(file),
        std::istream_iterator<char>());
	return (200);
}


std::string Request::getMethod()
{
	return (this->method);
}

std::vector<char> &Request::getBodyvec()
{
	return (bodyvec);
}

std::string Request::getHostname()
{
	return (hostname);
}

std::string Request::getPath()
{
	return (this->path);
}

int Request::getError()
{
	return (error);
}

std::string Request::getVer()
{
	return (this->ver);
}

std::string Request::getFull_path()
{
	return (full_path);
}

std::string Request::getNewlocation()
{
	return (newlocation);
}

bool Request::getRespchunked()
{
	return (respchunked);
}

void Request::setError(int error)
{
	this->error = error;
}

void Request::trimWhiteSpaces(std::string &line, bool end)
{
	std::string::size_type pos;
	if (end == false)
		pos = line.find_first_of(" ");
	else
		pos = line.find_last_of(" ");
	if (pos != std::string::npos)
		line.erase(pos, 1);
}



std::string Request::getContent()
{
	return(this->content);
}

void Request::constructResponse(std::vector<std::string> &dirContent, std::string &path)
{
	std::string response = "<html>"
	"<head>"
	"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
	"<title>Directory listing for /</title>"
	"</head>"
	"<body>"
	"<h1>Directory listing for " + path + "</h1>"
	"<hr>"
	"<ul>";
	std::string footer = "</ul>"
	"<hr>"
	"</body>"
	"</html>";
	for (std::vector<std::string>::iterator it = dirContent.begin(); it != dirContent.end(); ++it)
	{
		std::string tmp = "<li><a href=\"" + path + "/" + *it + "\">" + *it + "</a></li>"; // if not first_of_slash
		response.append(tmp);
	}
	response.append(footer);
	content = response;
}

void Request::uploadFile(std::vector<char> vec, std::size_t pos)
{
	try
	{
		std::string boundary;
		std::string tmp3;
		std::string filename;
		std::size_t j = 0, l = 0, start;

		while (vec[pos] != '\r' and vec[pos + 1] != '\n')
			boundary.append(1, vec[pos++]);
		pos += 2;
		while (vec[pos] != '\r' and vec[pos + 1] != '\n')
			tmp3.append(1, vec[pos++]);
		std::string tmp4(tmp3.substr(tmp3.find_last_of("=") + 2));
		filename = tmp4.substr(0, tmp4.size() - 1);
		tmp3.clear();
		pos += 2;
		while (vec[pos] != '\r' and vec[pos + 1] != '\n')
			tmp3.append(1, vec[pos++]);
		if (std::strncmp(tmp3.c_str(), "Content-Type:", 13) != 0)
			return ;
		pos += 4;
		start = pos;
		std::fstream file;
		file.open(configVec[idx].upload + "/" + filename, std::ios::app);
		while (true)
		{
			j = pos;
			l = 0;
			while (true)
				if (vec.at(j) == boundary[l]){j++;l++;} else
					break;
			if (l == boundary.length())
					break;
			pos++;
		}
		std::ostream_iterator<char> itr(file);
    	copy(vec.begin() + start, vec.begin() + pos, itr);
		file.close();
		if (vec[j] != '-' and vec[j + 1] != '-')
			uploadFile(vec, pos);
		else
			return;
	}
	catch(std::exception const &e)
	{
		error = CONTENTLENERR;
		return;
	}
}

std::string Request::decodeUrl(std::string &url)
{
	std::string decode;
	char ch;
	int ii;
	for (std::size_t i = 0; i < url.length(); i++)
	{
		if (url[i] == '+')
			decode += ' ';
		if (url[i] == '%')
		{
			std::sscanf(url.substr(i + 1,2).c_str(), "%x", &ii);
			ch=static_cast<char>(ii);
			decode+=ch;
			i = i + 2;
		} else {
			  decode += url[i];
		}
	}
	return (decode);
}

Request & Request::operator=(const Request &assign)
{
	this->idx = assign.idx;
	this->usedPort = assign.usedPort;
	this->configVec = assign.configVec;
	this->headers = assign.headers;
	this->buffer = assign.buffer;
	this->method = assign.method;
	this->path = assign.path;
	this->ver = assign.ver;
	this->content = assign.content;
	this->fullQuery = assign.fullQuery;
	this->body = assign.body;
	this->postContentType = assign.postContentType;
	this->raw = assign.raw;
	this->hostname = assign.hostname;
	this->socketfd = assign.socketfd;
	this->chunked = assign.chunked;
	this->end = assign.end;
	this->error = assign.error;
	this->newlocation = assign.newlocation;
	this->respchunked = assign.respchunked;
	this->bodyvec = assign.bodyvec;
	this->cookie = assign.cookie;
	return (*this);
}

Request::~Request()
{

}
