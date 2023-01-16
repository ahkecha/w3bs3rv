/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:39:23 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 16:32:30 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/webserv.hpp"
#include "../include/Config.hpp"
#include "../include/struct.hpp"

void shutdown(int sig)
{
	if (sig == SIGINT)
	{
		std::cout << std::endl << "Good bye!" << std::endl;
		std::exit(EXIT_SUCCESS);
	}
}

int main(int ac, char **av)
{
	signal(SIGINT, shutdown);
	if (ac == 2 or ac == 1)
	{
		std::vector<s_server> config;
		{
			std::string confFile;
			if (ac == 1)
			{
				confFile = "config/default.conf";
				std::cout << "\033[33;3m[WARNING] No config file supplied\033[0m\nUsing: \033[31;3m" << confFile << "\033[0m as default!" << std::endl;
			}
			else
				confFile = av[1];
			const parse conf(confFile);
			config = conf.getVector();
		}
		WebServ server(config);
    }
	return (1);
}
