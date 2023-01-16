/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:39:30 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 11:10:33 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __STRUCT_H__
#define __STRUCT_H__

#include <string>
#include <vector>
#include <set>

struct s_location{
	std::string 					Path;
	std::string 					deny;
	std::string 					allow;
	std::string 					cgiExt;
	std::string 					cgiPath;
	std::string 					root;
	char 							autoindex;
	std::string 					index;
	std::string 					redirect;
	std::string 					allowed_methods;
	
	void clear()
	{
		Path.clear();
		deny.clear();
		allow.clear();
		cgiExt.clear();
		cgiPath.clear();
		root.clear();
		index.clear();
		redirect.clear();
		allowed_methods.clear();
	}
};
struct s_listen
{
	std::string 					host;
	std::string 					port;
};
struct s_error
{
	std::string 					error_path;
	std::string 					content;
	int								error;
};
struct s_server{
	s_listen 						listen;
	std::vector<std::string> 		server_name;
	std::string 					root;
	std::string 					index;
	bool							autoindex;
	std::string 					error_page;
	std::vector <s_error> 			errvec;
	std::string 					allowed_methods;
	std::string 					upload;
	std::string 					clientmaxbsize;
	std::vector<s_location> 		location;
	std::size_t						maxsize;
};

struct header
{
	std::string headers;
	std::string value;
	header()
	{
		headers = "";
		value = "";
	};
};

struct urlParams
{
	std::string param;
	std::string value;

};

struct mime
{
	std::string extension;
	std::string mimetype;
	int size()
	{
		return (347);
	}
};

struct s_client
{
	std::vector<char> 	data;
	time_t				stime;
	bool 				start;
};
struct bSlashes
{
	bool operator()(char a, char b) const
	{
		return (a == '/' and b == '/');
	}
};

enum Selement {Serror, listen_, server_name, index_,\
	root, autoindex, error_page, allowed_methods , \
	upload, location, client_max_body_size};
enum Lelement {Lerror, deny, allow, cgiExt, cgiPath,\
	locationRoot, locationAutoIndex, locationIndex, \
	locationRedirect, locationAllowedMethods};

#endif // __STRUCT_H__
