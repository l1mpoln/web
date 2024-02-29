/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_config.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 16:22:28 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/21 16:10:22 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"

std::string extractValue(const std::string& line) 
{
    size_t firstQuote = line.find('"');
    size_t lastQuote = line.rfind('"');
    if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote != lastQuote) {
        return line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
    }
    return "";
}

void ConfigParse::loadConfig(const std::string& configFileName) 
{
    std::ifstream configFile(configFileName.c_str());
    std::string line;
    ServerConfig currentServer;
    bool inServerBlock = false;
    int count = 0;

    while (std::getline(configFile, line)) 
    {
        if (line.empty())
            continue;
        if (line.find("server {") != std::string::npos)
        {
            if (count > 0)
            {
                currentServer = check_config_file(currentServer);
                servers.push_back(currentServer);
            }
            count++;
            currentServer = ServerConfig();
        }
        size_t delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) 
            continue;
        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = extractValue(line);
        if (key == "server_name") {
            currentServer._serverName = value;
        } 
        else if (key == "listen") {
            std::istringstream iss(value);
            std::string port;
            while (getline(iss, port, ' ')) {
                currentServer._listenPorts.push_back(port);
            }
        } else if (key == "autoindex") {
            currentServer._autoindex = value;
            
        } else if (key == "methods") {
            std::istringstream iss(value);
            std::string method;
            while (getline(iss, method, ' ')) {
                currentServer._methods.push_back(method);
            }
        } else if (key == "allow_methods") {
            std::istringstream iss_2(value);
            std::string allow_methods;
            while (getline(iss_2, allow_methods, ' ')) {
                currentServer._allow_methods.push_back(allow_methods);
            }
        }else if (key == "default") {
                currentServer._defaultPage = value;
        }
        else if (key == "client_max_body_size") 
        {
	        for (int i = 0; value[i]; ++i)
	        {
	        	if (isdigit(value[i]))
	        	{
	        		currentServer._clientMaxBodySize += value[i];
	        	}
	        }
        }
          else if (key == "timeout") 
        {
	        for (int i = 0; value[i]; ++i)
	        {
	        	if (isdigit(value[i]))
	        	{
	        		currentServer._timeout += value[i];
	        	}
	        }
        } 
        else if (key == "max_clients") {
            currentServer._maxClients = value;
        }
        else if (key == "index") {
             currentServer._index = value;
        }
        else if (key == "upload"){
            currentServer._upload = value;
        }
        else if (key == "error400"){
            currentServer._error400 = value;
        }
        else if(key == "error403"){
            currentServer._error403 = value;
        }
        else if(key == "error404"){
            currentServer._error404 = value;
        }
        else if(key == "error413"){
            currentServer._error413 = value;
        }
        else if(key == "error500"){
            currentServer._error500 = value;
        }
        else if(key == "error504"){
             currentServer._error504 = value;
         }
        else if(key == "script1"){
            currentServer._script1 = value;
        }
        else if(key == "script2"){
            currentServer._script2 = value;
        }
        else if(key == "script3"){
            currentServer._script3 = value;
        }
    }
    currentServer = check_config_file(currentServer);
    servers.push_back(currentServer);
}

std::string trim(const std::string& str)
{
    
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool check_file(std::string path)
{
    std::ifstream file(path.c_str());
    
    if (file)
    {
        file.close();
        return true;
    }
    return false;
}

 ServerConfig check_config_file (ServerConfig currentServer)
 {
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator it_2;
    
    if (currentServer._listenPorts.empty())
    {
        std::cerr << "Bad Config from the Config File" << std::endl;
        exit(EXIT_FAILURE) ;
    }
    for (it = currentServer._listenPorts.begin(); it != currentServer._listenPorts.end(); ++it)
    {
        
        for (size_t i = 0; i < it->length(); ++i)
        {
            if (!isdigit((*it)[i]))
            {
               std::cerr << "False listen port" << std::endl;
                    exit(EXIT_FAILURE) ;
            }
            else
            {    
                if (atoi(it->data()) < 0 || atoi(it->data()) > 65535)
                   {
                    std::cerr << "False listen port" << std::endl;
                    exit(EXIT_FAILURE) ;
                   }
            }
        }
    }

    if (currentServer._serverName != "localhost")
    {
        currentServer._serverName = Default_Value_serverName;
    }
    if (currentServer._clientMaxBodySize.empty())
    {
        currentServer._clientMaxBodySize = Default_Value_clientMaxBodySize;
    }
    if (currentServer._timeout.empty())
    {
        currentServer._timeout = Default_Value_timeout;
    }
    if (currentServer._autoindex.empty() || (currentServer._autoindex != "on" && currentServer._autoindex != "off"))
    {
        currentServer._autoindex = Default_Value_autoindex;
    }
    if (!check_file(currentServer._defaultPage))
    {
        currentServer._defaultPage = Default_Value_defaultPage;
    }
    if (!check_file(currentServer._error400))
    {
        currentServer._error400 = Default_Value_error400;
    }
    if (!check_file(currentServer._error403))
    {
        currentServer._error403 = Default_Value_error403;
    }
    if (!check_file(currentServer._error404))
    {
        currentServer._error404 = Default_Value_error404;
    }
    if (!check_file(currentServer._error413))
    {
        currentServer._error413 = Default_Value_error413;
    }
    if (!check_file(currentServer._error500))
    {
        currentServer._error500 = Default_Value_error500;
    }
    if (!check_file(currentServer._error504))
    {
        currentServer._error504 = Default_Value_error504;
    }
    if (!check_file(currentServer._index))
    {
        currentServer._index = Default_Value_index;
    }
    if (!check_file(currentServer._script1))
    {
        currentServer._script1 = Default_Value_script1;
    }
    if (!check_file(currentServer._script2))
    {
        currentServer._script2 = Default_Value_script2;
    }
    if (!check_file(currentServer._script3))
    {
        currentServer._script3 = Default_Value_script3;
    }
    if (!check_file(currentServer._upload))
    {
        currentServer._upload = Default_Value_upload;
    }
    if (currentServer._methods.empty())
    {
        currentServer._methods.push_back(Default_Value_methods);
    }
    else
    {
        for (it_2 = currentServer._methods.begin(); it_2 != currentServer._methods.end(); ++it_2)
        {
            if (*it_2 != "GET" && *it_2 != "POST" && *it_2 != "DELETE")
            {
                std::cerr << "Error Config file: Bad allow method" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    if (currentServer._allow_methods.empty())
    {
        currentServer._allow_methods.push_back(Default_Value_allow_methods);
    }
    else
    {
        for (it_2 = currentServer._allow_methods.begin(); it_2 != currentServer._allow_methods.end(); ++it_2)
        {
            if (*it_2 != "GET" && *it_2 != "POST" && *it_2 != "DELETE")
            {
                std::cerr << "Error Config file: Bad allow method" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    if (currentServer._maxClients.empty())
    {
        currentServer._maxClients = Default_Value_maxClients;
    }
    return currentServer;
 }
