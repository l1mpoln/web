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

std::string handleFileUpload(int clientSocket, std::istringstream& requestStream, const std::string& htmlContent) {
    size_t contentLength = htmlContent.length();
    std::string requestBody;
    std::string requestLine;

    std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    // Читаем заголовки запроса
    while (std::getline(requestStream, requestLine) && requestLine != "\r") {
        std::cerr << requestLine << std::endl;
    }
    std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    // Получение длины тела запроса
    std::cout << "Content length: ===" << contentLength << std::endl; // Отладочное сообщение

    if (contentLength == 0) {
        std::cout << "Error: Empty request body" << std::endl; // Отладочное сообщение
        return "Error: Empty request body";
    }

    // Читаем тело запроса
    std::getline(requestStream, requestBody, '\0');

    // Поиск строки, содержащей "filename="
    size_t filenamePos = requestBody.find("filename=\"");
    if (filenamePos == std::string::npos) {
        std::cout << "Error: Filename not found in request" << std::endl;
        return "Error: Filename not found in request";
    }

    // Найдем начало имени файла
    filenamePos += 10; // Длина "filename=\"" равна 10 символам
    size_t filenameEnd = requestBody.find("\"", filenamePos);
    if (filenameEnd == std::string::npos) {
        std::cout << "Error: Unable to find end of filename" << std::endl;
        return "Error: Unable to find end of filename";
    }

    // Получим имя файла
    std::string filename = requestBody.substr(filenamePos, filenameEnd - filenamePos);

    // Получаем начало и конец сегмента данных файла
    size_t fileDataPos = requestBody.find("\r\n\r\n");
    if (fileDataPos == std::string::npos) {
        std::cout << "Error: Unable to find beginning of file data" << std::endl;
        return "Error: Unable to find beginning of file data";
    }
    fileDataPos += 4; // Пропускаем "\r\n\r\n"
    std::string fileData = requestBody.substr(fileDataPos); // Получаем данные файла

    // Сохраняем данные в файл
    std::ofstream outputFile(filename.c_str(), std::ios::binary);
    if (!outputFile) {
        std::cout << "Error: Failed to create file" << std::endl;
        return "Error: Failed to create file";
    }
    outputFile << fileData;
    outputFile.close();

    // Возвращаем сообщение об успешной загрузке файла
    return "File uploaded successfully";
}