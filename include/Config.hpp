/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:38:38 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 14:09:23 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "struct.hpp"
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#define UNSPEC "UNSPECIFIED"
#include <unistd.h>
#include <dirent.h>

class parse
{
	private:
		std::vector<s_server> 					config_server;
		std::map<std::string, Selement> 		Selements;
		std::map<std::string, Lelement> 		Lelements;
		std::ifstream   						file;
    	std::string     						line;
    	std::string     						buffer;
		std::stringstream 						data_stream;
		size_t 									i;
		int										k;
		std::string 							key;
		std::string 							sec;
		size_t 									index;

	public:
		parse();
		parse(std::string &);
		void 									parsing();
		std::multimap<std::string, std::string> getConfig();
		void 									register_server_elements();
		void 									register_location_elements();
		std::string 							getBuffer() const;
		std::vector<struct s_server> 			getVector() const;
		parse 									&operator=(const parse&);
		void 									checkLocationBlock(s_location &);
		~parse();

};

#endif // __CONFIG_H__
