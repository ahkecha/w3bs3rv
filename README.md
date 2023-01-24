![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![macOS](https://img.shields.io/badge/mac%20os-000000?style=for-the-badge&logo=macos&logoColor=F0F0F0)
<h1 align="center">
  Webserv
  <br>
</h1>

This project presents a web server that has been developed from scratch in C++, utilizing a configuration file similar to that of the widely-used nginx. The web server incorporates a parser and lexer for interpreting the configuration file, allowing for a high degree of customization to suit the specific needs of the user.

The key features of this web server include:

* The use of I/O multiplexing to handle multiple clients concurrently, ensuring efficient and speedy handling of incoming requests.
* Support for Common Gateway Interface (CGI) on custom paths, enabling the execution of scripts and dynamic content.
* Compliance with industry standards by adhering to the relevant RFCs.
* The ability to handle cookies and sessions.
* Can handle request time-out.

## Building

To build this project you need `autoconf` installed on your computer.


```bash
# Clone this repository
$ git clone https://github.com/ahkecha/webserv

# Go into the repository
$ cd webserv

# Run configure script
$ ./configure

# build the executable
$ make -C build/

```

## :gear: Configuration

Webserv uses a custom configuration syntax, similar to nginx config files

### Example
```
server {
	listen 8685; # ip:port
	server_name webserv.42
	root www/root;
	index internet.html;
	autoindex on;
	upload www/root/uploads; #file upload directory
	error_page 404 www/root/custom_404.html; #custom error pages
	allowed_methods GET,POST;
	client_max_body_size 6000000;

	location /favicon.ico {
		deny all;
	}

	location /intra {
		redirect http://profile.intra.42.fr;
	}

	location /python {
		root www/root/python;
		autoindex off;
		index index.py;
		allowed_methods GET,POST;
		cgiExt py;
		cgiPath /usr/local/bin/python3.9;
	}

	location /perl {
		root www/root/perl;
		allowed_methods GET,POST;
		cgiExt pl;
		cgiPath cgi-bin/perl;
	}

	location /php {
		allowed_methods GET,POST;
		cgiPath cgi-bin/php-cgi;
		cgiExt php;
	}
 }

```
The configuration file above serves as an example of the various directives that can be utilized to tailor the web server to the specific needs of the user:
* `listen`: used to specify the IP address and port on which the server will listen for incoming requests.
* `root` : sets the default root directory for the server(can be overriden in specific locations).
* `index` : specifies the default file to be served when no specific file is requested.
* `autoindex` : (self explanatory) when set to "on" will automatically generate a directory listing when a directory is requested.
* `upload`: specify a directory for handling file uploads.
* `error_page`: allows for custom error pages to be served for specific error codes.
* `allowed_methods`:  specifies which HTTP methods are allowed for a given location.
* `location` which is used to specify specific parameters for a specific route, such as a separate root directory or allowed methods as well as CGI path and Extension.

In the example configuration file, specific locations have been defined for handling different types of content, such as "static" and "video" locations, as well as locations for executing scripts written in Python, Perl, and PHP using the "cgiExt" and "cgiPath" directives. Additionally, a location for handling login requests and file uploads has been defined, with the appropriate allowed methods specified.

## Usage

If no config file is supplied, webserv will use the default config file located in `config/default.conf`

```bash
$ ./webserv PATH_TO_CONFIG_FILE
```
---
Overall, this web server provides a high degree of flexibility and configurability, allowing for easy customization to suit the specific needs of the user. With its efficient handling of multiple clients and support for CGI, this web server is a robust and reliable solution for serving web content.
## Authors
<table>
  <tbody>
    <tr>
       <td align="center"><a href="https://github.com/ahkecha"><img src="https://avatars.githubusercontent.com/u/58378453?v=3?s=100" width="100px;" alt="avatar"/><br /><sub><b>ahkecha</b></sub></a><br /><a href="#" title="stuff"></a>
       <td align="center"><a href="https://github.com/C0M-M4ND0"><img src="https://avatars.githubusercontent.com/u/94039533?v=3?s=100" width="100px;" alt="avatar"/><br /><sub><b>oabdelha</b></sub></a><br /><a href="#" title="stuff"></a>
    </tr>
  </tbody>
</table> 
