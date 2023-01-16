/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:52:59 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/04 16:53:00 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(std::vector<s_server> config)
{
	try
	{
		sock = new Socket(config);
	}
	catch(const std::bad_alloc & e)
	{
		std::exit(EXIT_FAILURE);
	}

}

Socket * Server::getLSock()
{
	return (this->sock);
}

Server::~Server()
{
	delete (this->sock);
}
