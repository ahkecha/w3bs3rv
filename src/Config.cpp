/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:39:04 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 16:32:11 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"
#include "../include/Lexer.hpp"

parse::parse()
{
}

void parse::register_server_elements()
{
	Selements["error"]  =  Serror;
    Selements["listen"]   = listen_;
    Selements["server_name"] = server_name;
	Selements["index"] = index_;
    Selements["root"]   = root;
	Selements["autoindex"]   = autoindex;
	Selements["error_page"]   = error_page;
	Selements["allowed_methods"]   = allowed_methods;
	Selements["upload"]   = upload;
	Selements["location"]   = location;
	Selements["client_max_body_size"]  = client_max_body_size;
}

void parse::register_location_elements()
{
	Lelements["error"]  =  Lerror;
    Lelements["deny"] = deny;
    Lelements["allow"]   = allow;
	Lelements["cgiExt"] = cgiExt;
    Lelements["cgiPath"]   = cgiPath;
	Lelements["root"] = locationRoot;
    Lelements["autoindex"]   = locationAutoIndex;
    Lelements["index"]   = locationIndex;
    Lelements["redirect"]   = locationRedirect;
    Lelements["allowed_methods"]   = locationAllowedMethods;
}


parse::parse(std::string &confFile) : i(0), k(-1)
{
	try
	{
		std::size_t cmnt;
		{
			std::string filename = confFile;
			std::size_t dot = filename.find_last_of('.');
			if (dot != std::string::npos)
			{
				if (filename.substr(dot + 1) != "conf")
					throw(lexer::InvalidExtension());
			}
			else
				throw(lexer::InvalidExtension());
		}
		file.open(confFile);
		if (file.fail()) throw(lexer::ConfigFileNotFound());
		while (std::getline(file, line))
		{
			cmnt = line.find('#');
			if (cmnt != std::string::npos)
			{
				std::string tmp = line.substr(cmnt);
				line.erase(cmnt, tmp.length());
				if (line.find_first_not_of('\t') == line.npos)
					continue;
			}
			buffer.append(line);
			if (line.length() != 0)
				buffer.append("\n");
		}
		file.close();
		data_stream << buffer;
		lexer(*this);
		register_server_elements();
		register_location_elements();
		parsing();
	}
	catch(std::exception const &e)
	{
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void	parse::parsing()
{
	struct s_location tmp;
	std::vector<std::string> tmpCount;
	std::vector<std::string> tmpCount2;
	std::size_t colon;
	std::size_t trailingBracket;
	config_server.push_back(s_server());

	while (std::getline(data_stream, line))
	{
		if (!std::strncmp(const_cast<const char *>(line.c_str()) , "server {", 8))
		{
			if (++k > 0)
				break;
			continue;
		}
		else if (line == "}")
			continue;
		index = line.find(32);
		if (index == std::string::npos)
			throw(lexer::InvalidConfigFile());
		key = line.substr(1, index - 1);
		sec = line.substr(index + 1, line.size());
		std::size_t semicolon = sec.find(';');
		if (semicolon != std::string::npos)
			sec.erase(semicolon);
		lexer::isValidKey(key);
		if (key != "location" && key != "error_page")
			tmpCount.push_back(key);
		switch (Selements[key])
		{
			case Serror:
				throw(lexer::InvalidConfigFile());

			case listen_:
				colon = sec.find(':');
				if (colon != std::string::npos)
				{
					config_server[i].listen.host = sec.substr(0, colon);
					if (config_server[i].listen.host == "localhost")
						config_server[i].listen.host = "127.0.0.1";
					config_server[i].listen.port = sec.substr(colon + 1);
				}
				else
				{
					config_server[i].listen.host = UNSPEC;
					config_server[i].listen.port = sec;
				}
				break;

			case server_name:
				config_server[i].server_name = lexer::cppSplit(sec, ' ');
				break;

			case index_:
				config_server[i].index = sec;
				break;

			case root:
				config_server[i].root = sec;
				break;

			case autoindex:
				if (sec == "on")
					config_server[i].autoindex = true;
				else if (sec == "off")
					config_server[i].autoindex = false;
				else
					throw(std::runtime_error("Error:\nautoindex must be on/off"));
				break;

			case error_page: {
				s_error tmperr;
				config_server[i].error_page = sec;
				std::vector<std::string> tmpvec = lexer::cppSplit(sec, ' ');
				if (tmpvec.size() == 2)
				{
					try
					{
						if (tmpvec[0].find_first_not_of("0123456789") != std::string::npos)
							throw(std::runtime_error("42"));
						tmperr.error = std::stoi(tmpvec[0]);
					}
					catch(...)
					{
						throw(lexer::InvalidConfigFile());
					}
					if (tmperr.error < 400 || tmperr.error > 505) throw(lexer::InvalidErrorPage());
					tmperr.error_path = tmpvec[1];
				}
				file.clear();
				file.open(tmperr.error_path );
				if (file.fail()) throw(lexer::InvalidErrorFile());
				line.clear();
				while (std::getline(file, line))
					tmperr.content.append(line);
				config_server[i].errvec.push_back(tmperr);
				file.close();
				break;
			}

			case allowed_methods:
				lexer::checkMethods(sec, ',');
				config_server[i].allowed_methods = sec;
				break;

			case upload:
				DIR *tmp2;
				if ((tmp2 = opendir(sec.c_str())) == NULL)
					throw(lexer::InvalidConfigFile());
				closedir(tmp2);
				config_server[i].upload = sec;
				break;

			case client_max_body_size:
				config_server[i].clientmaxbsize = sec;
				try
				{
					if (sec.find_first_not_of("0123456789") != std::string::npos)
						throw(std::runtime_error("42"));
					config_server[i].maxsize = std::stoul(sec);
				}
				catch(...)
				{
					throw(lexer::InvalidConfigFile());
				}
				break;

			case location:
				tmp.clear();
				tmp.autoindex = -1;
				trailingBracket = sec.find('{');
				if (trailingBracket != std::string::npos)
					tmp.Path = sec.substr(0, trailingBracket);
				trailingBracket = sec.find(' ');
				if (trailingBracket != std::string::npos)
					tmp.Path = sec.substr(0, trailingBracket);
				else
					tmp.Path = sec;
				while (std::getline(data_stream, line))
				{
					if (line == "\t}")
						break;
					index = line.find(32);
					key = line.substr(2, index - 2);
					sec = line.substr(index + 1);
					sec = sec.substr(0, sec.find(';'));
					tmpCount2.push_back(key);
					switch (Lelements[key])
					{
						case Lerror:
							throw(std::runtime_error("Error:\nInvalid Config File"));

						case deny:
							tmp.deny = sec;
							break;

						case allow:
							tmp.allow = sec;
							break;

						case cgiExt:
							tmp.cgiExt = sec;
							break;

						case cgiPath:
							if (std::fopen(sec.c_str(), "r") == NULL)
								throw(lexer::InvalidCgiPath());
							tmp.cgiPath = sec;
							break;

						case locationRoot:
							tmp.root = sec;
							break;

						case locationAutoIndex:
							if (sec == "on")
								tmp.autoindex = true;
							else if (sec == "off")
								tmp.autoindex = false;
							else
								throw(std::runtime_error("Error:\nautoindex must be on/off"));
							break;

						case locationRedirect:
							tmp.redirect = sec;
							break;

						case locationIndex:
							tmp.index = sec;
							break;

						case locationAllowedMethods:
							tmp.allowed_methods = sec;
							break;

					}
			}
			lexer::isDuplicate(tmpCount2);
			tmpCount2.clear();
			checkLocationBlock(tmp);
			config_server[i].location.push_back(tmp);
		}
		if(config_server[i].clientmaxbsize.empty())
			config_server[i].maxsize = -1;
	}
	lexer::isDuplicate(tmpCount);
	tmpCount.clear();
	if (k > 0)
	{
		k--;
		i++;
		parsing();
	}
	lexer::isValidValues(this->config_server);
}

std::string parse::getBuffer() const
{
	return (this->buffer);
}

std::vector<struct s_server> parse::getVector() const
{
	return (this->config_server);
}

parse &parse::operator=(const parse &assign)
{
	this->config_server = assign.getVector();
	return (*this);
}

void parse::checkLocationBlock(s_location &location)
{
	if (location.allow.empty() and \
		location.root.empty() and \
		location.deny.empty() and \
		location.index.empty() and \
		location.cgiExt.empty() and \
		location.cgiPath.empty() and \
		location.autoindex == -1 and \
		location.redirect.empty() and \
		location.allowed_methods.empty())
		throw(lexer::InvalidConfigFile());
}

parse::~parse()
{

}
