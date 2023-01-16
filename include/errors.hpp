/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oabdelha <oabdelha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 15:44:04 by oabdelha          #+#    #+#             */
/*   Updated: 2022/12/05 11:07:07 by oabdelha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ERRORS_H__
#define __ERRORS_H__

#include <string>

std::string webserv_err_css = "<style type=\"text/css\">"
"@import url(https://fonts.googleapis.com/css?family=Merriweather);"
"@import url(https://fonts.googleapis.com/css?family=Dancing+Script);"
"h1 {"
"   font-family: \"Merriweather\", sans-serif;"
"   -webkit-font-smoothing: antialiased;"
" 	text-rendering: geometricPrecision;"
"}"
".bottom {"
"	font-family: \"Dancing Script\", sans-serif;"
"	font-size: 1.5em;"
"</style>"
;

std::string webserv_bottom =
"<hr><center class=\"bottom\">Webserv</center>"
"</body>"
"</html>"
;

std::string webserv_400_page =
"<html>"
"<head><title>400 Bad Request</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>400 Bad Request</h1></center>"
+ webserv_bottom
;

std::string webserv_403_page =
"<html>"
"<head><title>403 Forbidden</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>403 Forbidden</h1></center>"
+ webserv_bottom
;

std::string webserv_404_page =
"<html>"
"<head><title>404 Not Found</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>404 Not Found</h1></center>"
+ webserv_bottom
;

std::string webserv_405_page =
"<html>"
"<head><title>405 Not Allowed</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>405 Not Allowed</h1></center>"
+ webserv_bottom
;

std::string webserv_408_page =
"<html>"
"<head><title>408 Request Time-out</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>408 Request Time-out</h1></center>"
+ webserv_bottom
;

std::string webserv_411_page =
"<html>"
"<head><title>411 Length Required</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>411 Length Required</h1></center>"
+ webserv_bottom
;

std::string webserv_413_page =
"<html>"
"<head><title>413 Request Entity Too Large</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>413 Request Entity Too Large</h1></center>"
+ webserv_bottom
;

std::string webserv_494_page =
"<html>"
"<head><title>400 Request Header Or Cookie Too Large</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>400 Bad Request</h1></center>"
"<center>Request Header Or Cookie Too Large</center>"
+ webserv_bottom;

std::string webserv_500_page =
"<html>"
"<head><title>500 Internal Server Error</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>500 Internal Server Error</h1></center>"
+ webserv_bottom
;

std::string webserv_501_page =
"<html>"
"<head><title>501 Not Implemented</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>501 Not Implemented</h1></center>"
+ webserv_bottom
;

std::string webserv_505_page =
"<html>"
"<head><title>505 HTTP Version Not Supported</title></head>"
+ webserv_err_css +
"<body>"
"<center><h1>505 HTTP Version Not Supported</h1></center>"
+ webserv_bottom
;

#endif // __ERRORS_H__
