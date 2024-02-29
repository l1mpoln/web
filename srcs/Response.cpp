/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 17:36:32 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/21 13:31:30 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"

template <typename T>
std::string my_to_string(T value) 
{
    std::ostringstream os;
    os << value;
    return os.str();
}

string WebServer::sendHttpResponse(int clientSocket, const std::string& htmlContent) 
{
    string response_final;
    ostringstream response;

    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html; charset=UTF-8\r\n";
    response << "Content-Length: " << htmlContent.length() << "\r\n";
    response << "\r\n";
    
    response << htmlContent;
    response_final = response.str();
    return response_final;
}


string WebServer::sendTextResponse(int clientSocket, const std::string& message) 
{
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + my_to_string(message.size()) + "\r\n\r\n" + message;
    return response;
}


string WebServer::sendNotFoundResponse(int clientSocket)
{
    cerr << "Error 404: Page Not Found" << endl;
    std::string filename = socketToServerConfigMap[clientSocket]._error404;
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: " + my_to_string(content.size()) + "\r\n\r\n" + content;
        file.close();
        return response;
    }
    return ("HTTP/1.1 404 Not Found\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error404Content.length()) + "\r\n"
            "\r\n" + error404Content);
}

string WebServer::sendBadRequestResponse(int clientSocket) 
{
    std::string filename = socketToServerConfigMap[clientSocket]._error400;
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: " + my_to_string(content.size()) + "\r\n\r\n" + content;
        file.close();
        return (response);
    }
    return ("HTTP/1.1 400 Bad Request\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error404Content.length()) + "\r\n"
            "\r\n" + error404Content);
}

string WebServer::sendError413(int clientSocket) 
{
    std::string filename = socketToServerConfigMap[clientSocket]._error413;
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::string response = "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\nContent-Length: " + my_to_string(content.size()) + "\r\n\r\n" + content;
        file.close();
        return response;
    }
    return ("HTTP/1.1 413 Payload Too Large\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error404Content.length()) + "\r\n"
            "\r\n" + error404Content);
    
}
string WebServer::sendError403(int clientSocket) 
{
    std::cerr << "Error 403 Forbidden.\n";
    std::string filename = socketToServerConfigMap[clientSocket]._error403;
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::string response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: " + my_to_string(content.size()) + "\r\n\r\n" + content;
        file.close();
        return response;
    }
    return ("HTTP/1.1 403 Internal Server Error\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error403Content.length()) + "\r\n"
            "\r\n" + error403Content);
}
string WebServer::sendError500(int clientSocket) 
{
    std::cerr << "Unable to open the output file.\n";
    std::string filename = socketToServerConfigMap[clientSocket]._error500;
    std::ifstream file(filename.c_str());
    if (file.is_open()) {
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: " + my_to_string(content.size()) + "\r\n\r\n" + content;
        file.close();
        return response;
    }
    return ("HTTP/1.1 500 Internal Server Error\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error500Content.length()) + "\r\n"
            "\r\n" + error500Content);
}

string WebServer::sendError408() 
{
    return ("HTTP/1.1 408 Request Timeout\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error408Content.length()) + "\r\n"
            "\r\n" + error408Content);
}

string WebServer::sendError405() 
{
    return ("HTTP/1.1 405 Method not Allowed\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error405Content.length()) + "\r\n"
            "\r\n" + error405Content);
}

string WebServer::sendError505(int clientSocket)
{
    std::cerr << "HTTP Version Not Supported\n";
    return ("HTTP/1.1 505 version Mot Supported\r\n Content-Type: text/html\r\n"
            "Content-Length: " + my_to_string(error505Content.length()) + "\r\n"
            "\r\n" + error505Content);
}

string WebServer::sendFileResponse(int clientSocket, const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (file.is_open()) 
    {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        unsigned int port = clientSocketToPortMap[clientSocket];
        std::ostringstream portStream;
        portStream << port;
        std::string portStr = portStream.str();

        std::string::size_type pos = content.find("{PORT}");
        while (pos != std::string::npos) {
            content.replace(pos, 6, portStr);
            pos = content.find("{PORT}", pos + portStr.length());
        }

        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
                       << content.size() << "\r\n\r\n" << content;
        std::string response = responseStream.str();    
        return response;
    } 
    else 
    {
        return (sendNotFoundResponse(clientSocket));
    }
}

string WebServer::sendCssResponse(int clientSocket)
{
    std::string cssFilePath = "html/data/style.css";
    std::ifstream cssFile(cssFilePath.c_str());
    if (cssFile.is_open()) {
        std::string cssContent((std::istreambuf_iterator<char>(cssFile)),
                                std::istreambuf_iterator<char>());
        cssFile.close();
        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nContent-Length: " << cssContent.size() << "\r\n\r\n" << cssContent;
        return responseStream.str();
    }
    else 
    {  
        return sendNotFoundResponse(clientSocket);
    }
}

string WebServer::sendSessionResponse(int clientSocket, const std::string& filename)
{
    ifstream file(filename.c_str());
    string id;
    
    if (file.is_open()) 
    {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        unsigned int port = clientSocketToPortMap[clientSocket];
        ostringstream portStream;
        portStream << port;
        string portStr = portStream.str();
   
        if (!_username.empty())
            _username = _username.substr(1);
        ostringstream tmp_id;
        tmp_id << _username << "\n\n" << "Cookie session: " << _cookies["sessionId"] << "\n";
        string id = tmp_id.str();
        size_t poss = content.find("{USERNAME}");
        while (poss != std::string::npos) {
            content.replace(poss, id.length(), id);
            poss = content.find("{USERNAME}", poss + id.length());
        }
        
        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "
                       << content.size() << "\r\n\r\n" << content;
        std::string response = responseStream.str();    
        return response;
    } 
    else 
    {
        return (sendNotFoundResponse(clientSocket));
    }
}

string WebServer::handleLogout(int clientSocket)
{
    std::string setCookieHeader = "Set-Cookie: sessionId=; Path=/; HttpOnly; Secure\r\n";
    std::string locationHeader = "Location: /logout\r\n";
    std::string contentLength = "Content-Length: 0\r\n";
    return "HTTP/1.1 302 Found\r\n" + setCookieHeader + locationHeader + contentLength + "\r\n";
}

