/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session_cookie.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 17:48:02 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/20 18:51:53 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"
#include <ctime>

std::string generateNewSessionId()
{
    static long counter = 0;
    std::ostringstream ss;
    ss << std::time(0) << "_" << ++counter;
    return ss.str();
}

string WebServer::handleLogin(int clientSocket, std::istringstream& requestStream)
{
    string body;
    string username;
    string password;
    string line;
    int j = 0;
    
    while (std::getline(requestStream, line)) 
    {
        if (line.find("username") != string::npos)
            body = line;
    }
    for (int i = 0; i < body.length(); ++i)
    {
        
        if (body[i] == '=' && j == 0)
        {
            while (body[i] != '&')
            {
                username = username + body[i];
                i++;
            }
            j = 1;
                
        }
        if (body[i] == '=' && j == 1)
        {
            while (body[i] != '\0')
            {
                password = password + body[i];
                i++;
            }
                
        }
    }
    if (!username.empty() && !password.empty())
    {    
        return (handleSession(clientSocket, _cookies, username));
    }
    else
    {
        return (sendFileResponse(clientSocket, "data/login.html"));
    }
    
}

string WebServer::handleSession(int clientSocket, std::map<std::string, std::string>& cookies, const string &username)
{
    std::string sessionId = cookies["sessionId"];

    if (sessionId.empty() || activeSessions.find(sessionId) == activeSessions.end()) {
        sessionId = generateNewSessionId();
        SessionData newSession;
        _username = username;
        newSession.username = username;
        activeSessions[sessionId] = newSession;

        std::string setCookieHeader = "Set-Cookie: sessionId=" + sessionId + "; Path=/; HttpOnly; Secure\r\n";
        std::string locationHeader = "Location: /home\r\n";
        std::string contentLength = "Content-Length: 0\r\n";
        return "HTTP/1.1 302 Found\r\n" + setCookieHeader + locationHeader + contentLength + "\r\n";
    } 
    else 
    {
        std::string username = activeSessions[sessionId].username;
    }
    return "";
}
