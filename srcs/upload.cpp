/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/02 17:40:56 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/13 14:02:14 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"

string WebServer::handleFileUpload(int clientSocket, std::istringstream& requestStream) 
{
    int contentLength = 0;
    std::string line;
    
    while (std::getline(requestStream, line)) 
    {
        if (line == "\r") 
            break;
        else if (line.find("Content-Length:") != std::string::npos) 
            contentLength = atoi(line.substr(line.find(":") + 1).c_str());
    }
    std::cerr << "Content-Length: " << contentLength << std::endl;
    std::ostringstream fileData;
    char buffer[1024];
    int bytesRead = 0;
    int totalBytesRead = 0;
    while (totalBytesRead < contentLength) 
    {
        bytesRead = recv(clientSocket, buffer, (sizeof(buffer) < (contentLength - totalBytesRead)) ? sizeof(buffer) : (contentLength - totalBytesRead), 0);
        if (bytesRead <= 0) 
        {
            std::cerr << "Error reading file data.\n";
            std::string response = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to read file data.\n";
            return sendError500(clientSocket);
        }
        totalBytesRead += bytesRead;
        fileData.write(buffer, bytesRead);
    }
    if (totalBytesRead == contentLength) 
    {
        std::ofstream outputFile("./Upload_File/uploaded_file.txt", std::ios::binary);
        if (outputFile.is_open()) 
        {
            outputFile << fileData.str();
            outputFile.close();
            std::cerr << "File uploaded successfully.\n";
            std::string response = "HTTP/1.1 200 OK\r\n\r\nFile uploaded successfully.\n";
            return response;
            //send(clientSocket, response.c_str(), response.size(), 0);
        } 
        else 
            return sendError500(clientSocket);
    }
    return 0;
}