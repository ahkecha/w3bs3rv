/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:07 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 11:20:07 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Lexer.hpp"
#include "../include/Config.hpp"

std::string lexer::keys[] = {"client_max_body_size", "root",\
	"index", "server_name", "listen", "location", "cgiPath",\
	"cgiExt", "autoindex", "error_page", "allowed_methods", \
	"upload", "redirect"};

lexer::lexer(const parse &config): allowedChars("/,;#.{}_-:")
{
	CheckInvalidId(config);
}

void lexer::CheckInvalidId(const parse &config) const
{
	if (config.getBuffer().length() <= 0)
		throw(lexer::InvalidConfigFile());
	const std::string tmp = config.getBuffer();
	std::string tmp2;
	std::stringstream tmpstream;
	std::vector<std::string> tmpvec;
	int i = 1;
	int tabs = 0;
	int srv = 0;

	std::string::const_iterator it = tmp.cbegin();
	std::string::const_iterator ite = tmp.cend();
	while (it != ite)
	{
		if (*it == '\n')
			i++;
		if (!isalnum(*it) && !isspace(*it) && allowedChars.find(*it)\
			== std::string::npos)
			throw (lexer::InvalidConfigFile());
		++it;
	}
	tmpstream << config.getBuffer();
	while (std::getline(tmpstream, tmp2))
	{
		if (tmp2.compare("server {") == 0) srv++;
		if (!srv) throw(lexer::InvalidConfigFile());
		if (tabs < 0)
			throw(lexer::InvalidConfigFile());
		if (lexer::isEmpty(tmp2))
			throw(lexer::InvalidConfigFile());
		if (tmp2.back() == '}' && lexer::isOne(tmp2, '}') == 1)
			tabs--;
		for (std::size_t i = 0; i < tmp2.size(); i++)
		{
			if (tmp2[i] == 32 && tmp2[i + 1] == 32)
				throw(lexer::InvalidConfigFile());
		}
		if (tabs)
		{
			if (static_cast<std::size_t>(tabs) > tmp2.length())
				throw(lexer::InvalidConfigFile());
			if (tmp2.substr(0, tabs) != std::string(tabs, '\t')\
				|| std::count(tmp2.begin(), tmp2.end(), '\t') != tabs)
				throw(lexer::InvalidConfigFile());
			if((tmp2.back() != ';' && tmp2.back() != '{' && tmp2.back()\
				!= '}') || !lexer::isSemiColon(tmp2))
				throw(lexer::InvalidConfigFile());
		}
		if (tabs == 1)
		{
			tmpvec = lexer::cppSplit(tmp2, ' ');
			if (tmpvec[0] == "\tlocation")
			{
				if (tmpvec.size() != 3)
					throw(lexer::InvalidConfigFile());
			}
			else if (tmpvec[0] == "\tlisten")
				lexer::isListenValid(tmpvec);
			else
			{
				if (tmpvec.size() < 2 && tmp2 != "\t}")
					throw(lexer::InvalidConfigFile());
			}
			tmpvec.clear();
		}
		if(tabs == 2)
		{
			tmpvec = lexer::cppSplit(tmp2, ' ');
			if (tmpvec.size() != 2)
				throw(lexer::InvalidConfigFile());
			tmpvec.clear();
		}
		if (tmp2.back() == '{' && lexer::isOne(tmp2, '{') == 1)
			tabs++;
		for(std::string::iterator it = tmp2.begin(); it != tmp2.end(); ++it)
		{
			if (isalpha(*it))
				break;
			if (*it != '\t' && *it != '}')
				throw(lexer::InvalidConfigFile());
		}
		tmp2.clear();
	}
	if (lexer::isClosedBrackets(tmp) == false)
		throw(lexer::InvalidConfigFile());
}

std::vector<std::string> lexer::cppSplit(std::string &str, const char delim)
{
	std::stringstream tmp;
	std::string tmp1;
	std::vector<std::string> vec;
	tmp << str;
	while (std::getline(tmp, tmp1, delim))
	{
		vec.push_back(tmp1);
		tmp1.clear();
	}
	return (vec);
}

bool lexer::isEmpty(std::string &str)
{
    for(std::size_t i = 0; i < str.length(); i++)
	{
        if(!std::isspace(str[i]))
            return (false);
    }
    return (true);
}

bool lexer::isOne(std::string &str, const char c)
{
	std::size_t count = 0;
	for (std::size_t i = 0; i < str.length(); i++)
		if (str[i] == c)
			count++;
	if (count == 1)
		return (true);
	return (false);
}

bool lexer::isClosedBrackets(const std::string &fullFile)
{
	int count = 0;
	for (std::size_t i = 0; i < fullFile.length(); i++)
	{
		if (fullFile[i] == '{')
			count++;
		if (fullFile[i] == '}')
			count--;
	}
	return (count == 0);
}

bool lexer::isSemiColon(const std::string &str)
{
	std::size_t count = 0;
	for (std::size_t i = 0; i < str.length(); i++)
		if (str[i] == ';')
			count++;
	if (count <= 1)
		return (true);
	return (false);
}

void lexer::isListenValid(std::vector<std::string> &vec)
{
	std::size_t pos;
	if (vec.size() != 2)
		throw(lexer::InvalidConfigFile());
	if((pos = vec[1].find(':')) != std::string::npos)
	{
		if (!lexer::isOne(vec[1], ':'))
			throw(lexer::InvalidConfigFile());
	}
}

void lexer::isValidKey(std::string &key)
{
	int count = 0;
	for (int i = 0; i < 13; i++)
	{
		if (keys[i] == key)
		{
			count++;
			break;
		}
	}
	if (!count)
		throw(lexer::InvalidConfigFile());
}

void lexer::isDuplicate(std::vector<std::string> &keyList)
{
	std::sort(keyList.begin(), keyList.end());
	std::vector<std::string>::iterator it = std::unique(keyList.begin(), keyList.end());
	if (it != keyList.end())
		throw(lexer::DupException());
}

void lexer::isValidValues(std::vector<struct s_server> &configVector)
{
	std::vector<std::string> tmpVec;

	for(std::vector<struct s_server>::iterator it = configVector.begin(); it != configVector.end(); ++it)
	{
		if (it->listen.port.find_first_not_of("0123456789") != std::string::npos)
			throw(lexer::InvalidPort());
		if (std::stoi(it->listen.port) <= 0 || std::stoi(it->listen.port) > 65535)
			throw(lexer::InvalidPort());

		repeatedDots(it->listen.host);
		tmpVec = lexer::cppSplit(it->listen.host, '.');
		std::vector<std::string>::iterator it1 = tmpVec.begin();
		for (; it1 != tmpVec.end(); ++it1)
		{
			if (!it1->empty())
			{
				if (!(it1->compare(UNSPEC)))
					continue;
				if (tmpVec.size() != 4)
					throw(lexer::InvalidHost());
				if (it1->find_first_not_of("0123456789") != std::string::npos)
					throw(lexer::InvalidHost());
				try
				{
					if (std::stoi(*it1) < 0 || std::stoi(*it1) > 255)
						throw(lexer::InvalidHost());
				}
				catch(std::out_of_range &e)
				{
					(void)e;
					throw(lexer::InvalidHost());
				}
			}
		}
		std::vector<struct s_location>::iterator it2 = it->location.begin();
		for (; it2 != it->location.end(); ++it2)
		{
			if (it2->Path.front() != '/')
				throw(lexer::InvalidLocation());
		}
		tmpVec.clear();
	}
}

void lexer::checkMethods(std::string &str, char delim)
{
	if (str.front() == ',' or str.back() == ',')
		throw(lexer::InvalidConfigFile());
	for (std::size_t i = 0; i < str.length(); i++)
	{
		if ((str[i] == delim and str[i + 1] == \
			delim) or (str[i] == delim and str[i - 1] == delim))
			throw(lexer::InvalidConfigFile());
	}
	std::vector<std::string> tmp = lexer::cppSplit(str, ',');
	std::vector<std::string>::iterator it = tmp.begin();
	for (; it != tmp.end(); ++it)
	{
		if (it->compare("GET") and it->compare("POST")\
			and it->compare("DELETE"))
			throw(lexer::InvalidHTTPmethods());
	}
}


void lexer::repeatedDots(std::string &str)
{
	for (std::size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == '.' && ((str[i + 1] == '.') || str[i - 1] == '.'))
			throw(lexer::InvalidHost());
	}
	if (str[0] == '.' || str[str.length() - 1] == '.')
		throw(lexer::InvalidHost());
}

lexer::~lexer()
{

}