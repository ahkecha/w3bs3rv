/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:39:59 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 10:38:33 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Socket.hpp"

void printerr(const char *err)
{
	std::cout << "Error:" << std::endl << err << " " << std::strerror(errno) << std::endl;
	std::exit(EXIT_FAILURE);
}

Socket::Socket(std::vector<s_server> conf)
{
	try
	{
		sockets_fd = new int[conf.size() + 1];
		addrs = new sockaddr_in[conf.size() + 1];
	}
	catch(const std::bad_alloc& e)
	{
		std::exit(EXIT_FAILURE);
	}
	std::memset(addrs, 0, conf.size() + 1);
	std::memset(sockets_fd, 0, conf.size() + 1);
	std::size_t i = 0;
	for (; i < conf.size(); i++)
	{
		addrs[i].sin_family = AF_INET;
		addrs[i].sin_port = htons(std::stoi(conf[i].listen.port.c_str()));
		if (conf[i].listen.host == UNSPEC)
			addrs[i].sin_addr.s_addr = htonl(INADDR_ANY);
		else
			addrs[i].sin_addr.s_addr = inet_addr(conf[i].listen.host.c_str());
		sockets_fd[i] = socket(AF_INET, SOCK_STREAM, 0);
		if (sockets_fd[i] < 0)
			printerr("Socket");
		int on = 1;
		if (setsockopt(sockets_fd[i], SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
			printerr("Setsockopt");
		if (bind(sockets_fd[i], (struct sockaddr *)&addrs[i], sizeof(addrs[i])) < 0)
			printerr("Bind");
		if (listen(sockets_fd[i], 10) < 0)
			printerr("Listen");
		if (fcntl(sockets_fd[i], F_SETFL, O_NONBLOCK) < 0)
			printerr("Fcntl");
	}
	size = conf.size();
	sockets_fd[conf.size()] = 0;
	for (std::size_t i = 0; i < conf.size(); i++)
		std::cout << "Listening on port: \033[34;4m"<< conf[i].listen.port  << "\033[0m !" << std::endl;

}

int Socket::getSock()
{
	return (this->sock);
}

int Socket::getConnection()
{
	return (this->connection);
}

int *Socket::getSockets()
{
	return (this->sockets_fd);
}
struct sockaddr_in *Socket::getAddrs()
{
	return (this->addrs);
}
struct sockaddr_in Socket::getAddr()
{
	return (this->addr);
}

void Socket::setConnection(int so)
{
	this->connection = so;
}

Socket::~Socket()
{
	delete(this->sockets_fd);
	delete(this->addrs);
}
