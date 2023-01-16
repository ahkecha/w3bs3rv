/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:19 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 13:04:27 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include "Socket.hpp"
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <ostream>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string>
#include <poll.h>
#include <vector>
#include <map>

#define TIMEOUT 10000
#define READ_EVENT 1
#define WRITE_EVENT 2
#define KMAXEVENTS 20
#define CRLF2 "\r\n\r\n"


class Server
{
	private:
		Socket 			*sock;
	public:
		Server(std::vector<s_server>);
		Socket 			*getLSock();
		~Server();
};

#endif // __SERVER_H__
