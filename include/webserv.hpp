/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:38:29 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 15:07:59 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __WebServ_H__
#define __WebServ_H__

#include "Request.hpp"
#include "Server.hpp"
#include <dirent.h>
#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <sstream>
#include <time.h>

#define TIMEDOUT 846746
#define ENTITYTOOLARGE 2
#define CONTENTLENERR 1
#define DISABLE_WRITE 5
#define LENREQUIRED 411
#define ENABLE_WRITE 6
#define DISABLE_READ 7
#define ENABLE_READ 8
#define CRLF "\r\n"

class WebServ: public Server
{

	private:
		std::map<int, s_client> 			client;
		std::vector<s_server>				tmpConfig;
		std::string 						res;
		std::string 						mime;
		std::string 						port;
		Request								req;
		int w;

	public:
		WebServ(std::vector<s_server>);
		static std::vector<std::string> 	autoindex(std::string);
		void 								sendData(int);
		static void 						err(const char *);
		std::string 						error_page(int);
		std::string 						header(int);
		void 								evUpdate(int, int, int, int);
		void 								acceptClient(int, int);
		void 								recvData(int, u_long, int);
		int									ifFinish(std::vector<char> &);
		std::string 						buildResponse();
		int 								checkMethod();
		std::vector<std::string> 			getChunkedResponse();
		~WebServ();
};

#endif // __WebServ_H__
