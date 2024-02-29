/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/25 12:56:38 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/21 15:46:03 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#define Default_Value_serverName "localhost"
#define Default_Value_autoindex "on"
#define Default_Value_methods "GET"
#define Default_Value_defaultPage "./html/data/index.html"
#define Default_Value_clientMaxBodySize "10Mo"
#define Default_Value_maxClients "100"
#define Default_Value_index  "./html/data/index.html"
#define	Default_Value_upload "./html/data/upload_checker.html"
#define	Default_Value_error400 "./html/Error_Page/400.html"
#define	Default_Value_error403 "./html/Error_Paged/403.html"
#define	Default_Value_error404 "./html/Error_Page/404.html"
#define	Default_Value_error413 "./html/Error_Page/413.html"
#define	Default_Value_error500 "./html/Error_Page/500.html"
#define	Default_Value_error504 "./html/Error_Page/504.html"
#define	Default_Value_script1 "./html/cgi-bin/script_1.js"
#define	Default_Value_script2 "./html/cgi-bin/script_2.php"
#define	Default_Value_script3 "./html/cgi-bin/script_3.py"
#define Default_Value_allow_methods "GET POST"
#define Default_Value_timeout "100"

#include "ConfigParse.hpp"

class ServerConfig
{
    public:
		std::string _serverName;
    	std::vector<std::string> _listenPorts;
    	std::string _autoindex;
    	std::vector<std::string> _methods;
		std::vector<std::string> _allow_methods;
    	std::string _defaultPage;
    	std::string _clientMaxBodySize;
    	std::string _maxClients;
    	std::string _index; 
		std::string _upload;
		std::string _error400;
		std::string _error403;
		std::string _error404;
		std::string _error413;
		std::string _error500;
		std::string _error504;
		std::string _script1;
		std::string _script2;
		std::string _script3;
		std::string _timeout;
    
     	ServerConfig(){};
        ~ServerConfig(){};
};

#endif