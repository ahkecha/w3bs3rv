/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 16:53:40 by ahkecha           #+#    #+#             */
/*   Updated: 2022/12/05 10:49:53 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HEADERS_H__
#define __HEADERS_H__

#define CHUNKSIZE 128

#include <string>

std::string header_200 = "HTTP/1.1 200 OK\r\nServer: WebServ\r\nContent-Length:";
std::string header_200_chunked = "HTTP/1.1 200 OK\r\nServer: WebServ\r\nTransfer-Encoding: chunked";

std::string header_404 = "HTTP/1.1 404 NOT FOUND\r\nServer: WebServ\r\nContent-Length:";

std::string header_405 = "HTTP/1.1 405 Method not allowed\r\nServer: WebServ\r\nContent-Length:";

std::string header_403 = "HTTP/1.1 403 Forbidden\r\nServer: WebServ\r\nContent-Length:";
std::string header_501 = "HTTP/1.1 501 Not Implemented\r\nServer: WebServ\r\nContent-Length:";
std::string header_301 = "HTTP/1.1 301 Moved Permanently\r\nServer: WebServ\r\n";
std::string header_302 = "HTTP/1.1 302 Found\r\nServer: WebServ\r\n";
std::string header_307 = "HTTP/1.1 307 Temporary Redirect \r\nServer: WebServ\r\n";
std::string header_400 = "HTTP/1.1 400 Bad Request\r\nServer: WebServ\r\nContent-Length:";
std::string header_204 = "HTTP/1.1 204 No Content\r\nServer: WebServ\r\n\r\n";
std::string header_413 = "HTTP/1.1 413 Request Entity Too Large\r\nServer: WebServ\r\nContent-Length:";
std::string header_408 = "HTTP/1.1 408 Request Timeout\r\nServer: WebServ\r\nContent-Length:";

#endif // __HEADERS_H__
