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

std::string getBoundaryFromContentType(std::istringstream& requestStream) 
{
    std::string contentTypeHeader;
    std::getline(requestStream, contentTypeHeader);

    size_t pos = contentTypeHeader.find("boundary=");
    if (pos != std::string::npos) {
        return contentTypeHeader.substr(pos + 9);
    }

    return "";
}

std::string readPostData(std::istringstream& requestStream, const std::string& boundary)
{
    std::ostringstream postData;
    std::string line;

    // Пропускаем первую строку
    std::getline(requestStream, line);

    // Читаем данные, пока не встретим разделитель
    while (std::getline(requestStream, line) && line.find(boundary) == std::string::npos) {
        postData << line << "\n";
    }
    return postData.str();
}

std::string extractFilename(const std::string& formData) 
{
    size_t pos = formData.find("filename=\"");
    if (pos != std::string::npos) {
        pos += 10;  // Длина "filename=\""
        size_t endPos = formData.find("\"", pos);
        if (endPos != std::string::npos && endPos >= pos && endPos <= formData.size()) {
            return formData.substr(pos, endPos - pos);
        }
    }

    return "";
}


std::string extractFileContent(const std::string& formData, const std::string& boundary)
{
    size_t pos = formData.find(boundary);
    if (pos != std::string::npos) {
        pos += boundary.size();
        size_t endPos = formData.find(boundary, pos);
        if (endPos != std::string::npos && endPos >= pos && endPos <= formData.size()) {
            return formData.substr(pos, endPos - pos);
        }
    }

    return "";
}

void saveFile(const std::string& filename, const std::string& content) 
{
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

std::string WebServer::handleFileUpload(int clientSocket, std::istringstream& requestStream) 
{
    std::string boundary = getBoundaryFromContentType(requestStream);

    // Обработка данных, отправленных POST-запросом
    std::string formData = readPostData(requestStream, boundary);

    // Извлечение файла из формы
    std::string filename = extractFilename(formData);
    std::string fileContent = extractFileContent(formData, boundary);
    // Сохранение файла на сервере
    saveFile(filename, fileContent);

    // Возвращение ответа клиенту
    return sendTextResponse(clientSocket, "File uploaded successfully");
}