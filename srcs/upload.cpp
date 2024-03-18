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
#include <cstdlib>

std::string getFileNameFromHeader(const std::string& header) {
    size_t filenamePos = header.find("filename=\"");
    if (filenamePos == std::string::npos) {
        return ""; // Имя файла не найдено
    }

    filenamePos += 10; // Длина "filename=\"" равна 10 символам
    size_t filenameEnd = header.find("\"", filenamePos);
    if (filenameEnd == std::string::npos) {
        return ""; // Конец имени файла не найден
    }

    return header.substr(filenamePos, filenameEnd - filenamePos);
}

std::string handleFileUpload(int clientSocket, std::istringstream& requestStream, const std::string& htmlContent) {
    std::string line;
    std::string requestBody;
    bool foundContentType = false;

    // Пропускаем заголовки запроса
    while (std::getline(requestStream, line) && !line.empty()) {
        if (line.find("Content-Disposition") != std::string::npos) {
            size_t filenamePos = line.find("filename=\"");
            if (filenamePos != std::string::npos) {
                size_t filenameEnd = line.find("\"", filenamePos + 10); // Длина "filename=\"" равна 10 символам
                if (filenameEnd != std::string::npos) {
                    std::string filename = line.substr(filenamePos + 10, filenameEnd - (filenamePos + 10));
                    if (!filename.empty()) {
                        // Начинаем считывать данные файла
                        while (std::getline(requestStream, line) && line.find("------WebKitFormBoundary") == std::string::npos) {
                            if (line.find("Content-Type: text/plain") != std::string::npos) {
                                foundContentType = true;
                                continue;
                            }
                            if (foundContentType) {
                                requestBody += line + "\n";
                            }
                        }
                        // Удаляем последний символ новой строки, если он есть
                        if (!requestBody.empty() && requestBody[requestBody.length() - 1] == '\n') {
                            requestBody.erase(requestBody.length() - 1);
                        }
                        // Сохраняем данные в файл
                        std::ofstream outputFile(filename.c_str(), std::ios::binary);
                        if (!outputFile) {
                            std::cerr << "Error: Failed to create file" << std::endl;
                            return "Error: Failed to create file";
                        }
                        outputFile << requestBody;
                        outputFile.close();
                        return "File uploaded successfully";
                    }
                }
            }
        }
    }

    std::cerr << "Error: Content-Disposition header not found in request" << std::endl;
    return "Error: Content-Disposition header not found in request";
}