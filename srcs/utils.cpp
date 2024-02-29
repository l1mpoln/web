/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 17:44:19 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/21 13:27:59 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"
#include "../includes/ServerConfig.hpp"


void check_port(std::vector<ServerConfig> servers)
{
    std::set<std::string> uniquePorts;
    std::vector<ServerConfig>::iterator it;
    std::vector<std::string>::iterator it_port;

    for (it = servers.begin(); it != servers.end(); ++it) 
    {
        for (it_port = it->_listenPorts.begin(); it_port != it->_listenPorts.end(); ++it_port)
        {
            bool isInserted = uniquePorts.insert(*it_port).second;
            if (!isInserted) {
                std::cerr << "Error: Duplicate port found - " << *it_port << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

std::map<std::string, std::string> parseCookies(const std::string& cookieHeader) 
{
    std::map<std::string, std::string> cookies;
    std::istringstream stream(cookieHeader);
    std::string cookie;
    while (getline(stream, cookie, ';')) {
        size_t pos = cookie.find('=');
        if (pos != std::string::npos) {
            std::string name = cookie.substr(0, pos);
            std::string value = cookie.substr(pos + 1);
            cookies[name] = value;
        }
    }
    for (map<string, string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
 {
     std::cout << "Cookie haha "<< it->first << ": " << it->second << std::endl;
 }
    return cookies;
}

std::map<std::string, std::string> parseHeaders(const std::string& request)
{
    map<string, string> headers;
    istringstream requestStream(request);
    string line;
    while (getline(requestStream, line) && line != "\r") {
        size_t delimiterPos = line.find(": ");
        if (delimiterPos != string::npos) {
            string key = line.substr(0, delimiterPos);
            string value = line.substr(delimiterPos + 2);
            if (!value.empty() && value[value.size() - 1] == '\r') {
                value.erase(value.size() - 1);
            }
            headers[key] = value;
        }
    }
    return headers;
}

void WebServer::init_default__error_page()
{
    error404Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>404 Not Found</title>"
    "</head>"
    "<body>"
    "<h1>404 Not Found</h1>"
    "<p>The requested URL was not found on this server.</p>"
    "</body>"
    "</html>";
    error400Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>400 Bad Request</title>"
    "</head>"
    "<body>"
    "<h1>400 Bad Request</h1>"
    "<p>Your browser sent a request that this server could not understand.</p>"
    "</body>"
    "</html>";
    error403Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>403 Forbidden</title>"
    "</head>"
    "<body>"
    "<h1>403 Forbidden</h1>"
    "<p>You don't have permission to access this resource on this server.</p>"
    "</body>"
    "</html>";
    error413Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>413 Request Entity Too Large</title>"
    "</head>"
    "<body>"
    "<h1>413 Request Entity Too Large</h1>"
    "<p>The request is larger than the server is willing or able to process.</p>"
    "</body>"
    "</html>";
    error500Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>500 Internal Server Error</title>"
    "</head>"
    "<body>"
    "<h1>500 Internal Server Error</h1>"
    "<p>The server encountered an internal error or misconfiguration and was unable to complete your request.</p>"
    "</body>"
    "</html>";
    error504Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>504 Gateway Timeout</title>"
    "</head>"
    "<body>"
    "<h1>504 Gateway Timeout</h1>"
    "<p>The server did not receive a timely response from the upstream server or application.</p>"
    "</body>"
    "</html>";
    error408Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<title>408 Request Timeout</title>"
    "</head>"
    "<body>"
    "<h1>408 Request Timeout</h1>"
    "<p>The server timed out waiting for the request.</p>"
    "</body>"
    "</html>";
    error505Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<title>505 HTTP Version Not Supported</title>"
    "</head>"
    "<body>"
    "<h1>505 HTTP Version Not Supported</h1>"
    "<p>The server does not support the HTTP protocol version used in the request.</p>"
    "</body>"
    "</html>";
     error405Content =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='UTF-8'>"
    "<title>405 Method not Allowed</title>"
    "</head>"
    "<body>"
    "<h1>405 Method not Allowed</h1>"
    "</body>"
    "</html>";
    
}

void WebServer::checkTimeouts()
{
    time_t now = time(NULL);
    std::map<int, ClientInfo>::iterator it = clientTimeouts.begin();
    while (it != clientTimeouts.end())
    {
        if (now > it->second.expirationTime)
        {
            int fd = it->first;
            response_timeout[fd] = sendError408();
            for (size_t i = 0; i < fds.size(); ++i)
            {
                if (fds[i].fd == fd) {
                    fds[i].events = POLLOUT;
                    handle_outcoming_connection(i);
                    break;
                }
            }
            clientTimeouts.erase(it++);
        } 
        else
            ++it;
    }
}

