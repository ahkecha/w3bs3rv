/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:28 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 14:20:21 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sys/stat.h>

class parse;
class lexer
{
	private:
		const std::string 				allowedChars;
		static void 					repeatedDots(std::string &);

	public:
		static std::string 				keys[];
		class InvalidConfigFile: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid Config File!");
			}
		};
		class InvalidHTTPmethods: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid HTTP methods!");
			}
		};
		class InvalidExtension: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid Config File extension!");
			}
		};
		class ConfigFileNotFound: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nConfig File not found!");
			}
		};
		class InvalidLocation: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid Location");
			}
		};
		class DupException: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nDuplicate key in config file!");
			}
		};
		class InvalidPort: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid port!");
			}
		};
		class InvalidHost: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid Host!");
			}
		};
		class InvalidErrorPage: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid error page!");
			}
		};
		class InvalidErrorFile: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nError webpage not found!");
			}
		};
		class InvalidCgiPath: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\nInvalid CGI Path");
			}
		};
		class InvalidCgiExecutable: public std::exception
		{
			const char *what() const throw()
			{
				return ("Error:\n CGI binary is not executable");
			}
		};

		lexer(const parse &);
		void 							CheckInvalidId(const parse &) const;
		static std::vector<std::string> cppSplit(std::string &, const char);
		static bool 					isEmpty(std::string &);
		static bool 					isOne(std::string &, const char);
		static bool 					isClosedBrackets(const std::string &);
		static bool 					isSemiColon(const std::string &);
		static void 					isListenValid(std::vector<std::string> &);
		static void 					isValidKey(std::string &);
		static void 					isDuplicate(std::vector<std::string> &);
		static void 					isValidValues(std::vector<struct s_server> &);
		static void 					checkMethods(std::string &, char);
		~lexer();
};


#endif // __LEXER_H__
