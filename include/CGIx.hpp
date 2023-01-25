/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIx.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahkecha <ahkecha@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:44 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 12:52:57 by ahkecha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CGIx_H__
#define __CGIx_H__

#include "webserv.hpp"


class CGI
{
	private:
		std::vector<std::string> 		vec;
		std::string 				stringbuff;
		std::string 				host;
		std::string 				port;
		std::string 				method;
		std::string 				path;
		std::string 				query;
		std::string 				root;
		std::string 				cgiPath;
		std::string 				body;
		std::string 				type;
		std::string 				pathinfo;
		std::string 				cgiextension;
		std::string 				httpcookies;
		std::string				full_path;
		char 					*rawbody;

	public:
		bool 					contentLen;
		bool 					error;
		CGI(s_server &, s_location &, std::string &,\
			std::string &, std::string &, std::string &,\
			std::string &, char *, std::string &, std::string &,\
			std::map<std::string, std::string> &, std::string &);
		std::vector<std::string>		getCGIHeaders();
		std::string 				getContent();
		std::string 				getHeaders() const;
		std::string 				&getBody();
		void 					constructEnv();
		void 					executeCGI();
		int 					getStatusCode();
		~CGI();
};

#endif // __CGIx_H__
