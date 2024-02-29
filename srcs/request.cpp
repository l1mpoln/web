/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/16 13:43:52 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/21 16:20:39 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"

size_t WebServer::handle_incoming_connection(size_t i)
{
    if (isListeningSocket(fds[i].fd))
    {
        ClientInfo info;
        int clientSocket = accept(fds[i].fd, NULL, NULL);
        if (clientSocket < 0)
        {
            cout << "Error client socket\n";
            return i ;
        }
        
        info.socketFd = clientSocket;
        info.expirationTime = time(NULL) + atoi(socketToServerConfigMap[clientSocket]._timeout.c_str());
        clientTimeouts[clientSocket] = info;
        int flags = fcntl(clientSocket, F_GETFL, 0);
        if (flags == -1 || fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK) == -1) {
            cerr << "Error setting client socket to non-blocking mode\n";
            close(clientSocket);
            return i;
        }
    
         socketToServerConfigMap[clientSocket] = socketToServerConfigMap[fds[i].fd];
        clientSocketToPortMap[clientSocket] = listeningPortMap[fds[i].fd];
        _fd.fd = clientSocket;
        _fd.events = POLLIN;
        fds.push_back(_fd);
    }
    else 
    {
        char buffer[4096];
        int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            i = checkClientMaxBodySize(buffer, bytesRead, i);
        }
        else 
        {
            close(fds[i].fd);
            clientSocketToPortMap.erase(fds[i].fd);
            fds.erase(fds.begin() + i);
            --i;
        }
    }
    return i;
}

size_t WebServer::handle_outcoming_connection(size_t i)
{
    int fd = fds[i].fd;
    if (response_timeout.find(fd) != response_timeout.end() && _response.empty())
        _response = response_timeout[fd];
    if (!_response.empty())
    {
        ssize_t sentBytes = send (fds[i].fd, _response.c_str(), _response.length(), 0);
        if (sentBytes < 0)
        {
            close(fds[i].fd);
            clientSocketToPortMap.erase(fds[i].fd);
            fds.erase(fds.begin() + i);
            --i;
            cerr << "Error sending data" << endl;
        }
        else if (sentBytes < static_cast<ssize_t>(_response.length()))
        {
            _response = _response.substr(sentBytes);
            fds[i].events = POLLOUT;
        }
        else
        {
            cout << "Send() been completly send" << endl;
            if (_response.find("408 Request Timeout") != string::npos)
            {
              close(fds[i].fd);
              clientSocketToPortMap.erase(fds[i].fd);
              fds.erase(fds.begin() + i);
              --i;
              fds[i].events = POLLIN;
            }
            else if (_count == 0)
            {
                close(fds[i].fd);
                clientSocketToPortMap.erase(fds[i].fd);
                fds.erase(fds.begin() + i);
                --i;
                fds[i].events = POLLIN;
            }
            else
                 fds[i].events = POLLIN;
             _response.clear();
        }
        std::cerr << _request << std::endl;
        _request.clear();
    }
    return i;
}