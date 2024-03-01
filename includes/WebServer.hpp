/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 13:38:35 by vkuzmin           #+#    #+#             */
/*   Updated: 2024/02/21 13:31:36 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <sys/types.h>
#include <sys/wait.h>
#include "Utils.hpp"
#include "ConfigParse.hpp"
#include "ServerConfig.hpp"


struct SessionData {
    std::string username;
};

struct ClientInfo {
    int socketFd;
    time_t expirationTime;
};

class WebServer
{
    public:
        WebServer(std::string config_file_name);
        ~WebServer();
        void start();

    private:
        std::string sendInternalServerError(int clientSocket);
        string handleRequest(int clientSocket, const std::string& request);
        bool isListeningSocket(int fd);
        size_t checkClientMaxBodySize(char *buffer, int bytesRead, size_t i);
        size_t handle_incoming_connection(size_t i);
        size_t handle_outcoming_connection(size_t i);
        string handleSession(int clientSocket, std::map<std::string, std::string> &cookies, const string &username);
        string handleLogin(int clientSocket, std::istringstream& requestStream);

        string sendFileResponse(int clientSocket, const std::string& filename);
        string sendTextResponse(int clientSocket, const std::string& message);
        string sendNotFoundResponse(int clientSocket);
        string sendBadRequestResponse(int clientSocket);
        string sendSessionResponse(int clientSocket, const std::string& filename);
        string handleFileUpload(int clientSocket, std::istringstream& requestStream);
        string sendHttpResponse(int clientSocket, const std::string& htmlContent);
        string handleLogout(int clientSocket);
        string sendError413(int clientSocket);
        string sendError500(int clientSocket);
        string sendError408();
        string sendError505(int clientSocket);
        string sendError405();
        string sendError403(int clientSocket);
        string sendCssResponse(int clientSocket);

        void checkTimeouts();
        void init_default__error_page();
    
    private:
        int serverSocket;
        int serverPort;  //For config
        int _count;
        string _response;
        string _request;
        //std::string defaultPage;//For config
        //std::string error_page404;//For config
        
        map<int, ServerConfig> socketToServerConfigMap;
        map<int, unsigned int> listeningPortMap;
        map<int, unsigned int> clientSocketToPortMap;
        map<std::string, SessionData> activeSessions;
        map<std::string, std::string> _cookies;
        map<int, ClientInfo> clientTimeouts;
        map<int, string> response_timeout;
        string _username;
        
        pollfd _fd;
        ConfigParse config;
        
        set<int> listeningSocket;
        vector<pollfd> fds;
        
        string error400Content;
        string error403Content;
        string error404Content;
        string error413Content;
        string error500Content;
        string error504Content;
        string error408Content;
        string error505Content;
        string error405Content;

          

};

void check_port(std::vector<ServerConfig> servers);

#endif
