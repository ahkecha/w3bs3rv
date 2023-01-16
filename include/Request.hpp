/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:22 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 10:47:07 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "Lexer.hpp"

#define CR '\r'
#define LF '\n'
#define UNSPEC "UNSPECIFIED"
#define COOKIETOOLARGE 494

class Request
{
	private:
	
		std::map<std::string, std::string> 	cookie;
		std::string 						buffer;
		std::string 						method;
		std::string 						path;
		std::string 						ver;
		std::vector<struct header> 			headers;
		std::vector<struct urlParams> 		params;
		std::string 						content;
		std::vector<struct s_server> 		configVec;
		std::string 						usedPort;
		int 								idx;
		std::string							autoindexResp;
		std::string 						fullQuery;
		std::vector<std::string> 			cgiHeaders;
		std::string 						body;
		std::string 						postContentType;
		char 								*raw;
		bool								isBinary;
		std::size_t 						contentlen;
		int 								socketfd;
		std::vector<char> 					bodyvec;
		std::string 						newlocation;
		bool 								chunked;
		bool 								end;
		std::string 						hostname;
		std::string 						full_path;
		int 								error;
		bool 								respchunked;

	public:
		class InvalidContentLen: public std::exception
		{
			const char *what() const throw()
			{
				return ("\n");
			}
		};
		Request();
		Request(std::vector<char> &, std::vector<s_server>, std::string &, int);
		Request & operator=(const Request &);
		void 								trimWhiteSpaces(std::string &, bool);
		int 								CheckStatusLine() const;
		int 								CheckPath();
		std::string 						getMethod();
		std::string 						getPath();
		std::vector<char> 					&getBodyvec();
		std::string 						getVer();
		int									getError();
		void 								setError(int);
		std::string 						getContent();
		std::string 						getHostname();
		std::string 						getFull_path();
		bool 								getRespchunked();
		std::string 						getNewlocation();
		int 								getServer(std::string &) const;
		std::string 						decodeUrl(std::string &);
		void 								getUrlParams(std::string &);
		std::string  						getHeaderString();
		std::string  						getMimeType(std::string = UNSPEC);
		void 								constructResponse(std::vector<std::string> &, std::string &);
		void 								uploadFile(std::vector<char>, std::size_t);
		bool 								isChunked();
		int 								parseCookie(std::string &);
		~Request();
};

#endif // __REQUEST_H__
