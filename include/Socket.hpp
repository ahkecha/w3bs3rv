/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:40:08 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 10:38:11 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "../include/struct.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <cstdio>
#define UNSPEC "UNSPECIFIED"

class Socket
{
	private:
	
		int					sock;
		struct sockaddr_in 	addr;
		int 				connection;
		
	public:
	
		Socket(std::vector<s_server>);
		int 				size;
		int 				*sockets_fd;
		struct sockaddr_in 	*addrs;
		int 				*conns;
		int 				getSock();
		int 				getConnection();
		int 				*getSockets();
		struct sockaddr_in 	*getAddrs();
		struct sockaddr_in 	getAddr();
		void 				setConnection(int);
		~Socket();
};

#endif // __SOCKET_H__
