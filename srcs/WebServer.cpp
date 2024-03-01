/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 13:38:49 by vkuzmin           #+#    #+#             */
/*   Updated: 2024/02/23 11:20:41 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"
#include "../includes/ConfigParse.hpp"

const int MAX_BUFFER_SIZE = 1024;

WebServer::WebServer(std::string config_file_name) 
{
    vector<ServerConfig>::iterator it;
    
    config.loadConfig(config_file_name);
    init_default__error_page();
    check_port(config.servers);
    
    for(it = config.servers.begin(); it != config.servers.end(); ++it)
    {
        std::vector<std::string>::iterator portIt;
        for(portIt = it->_listenPorts.begin(); portIt != it->_listenPorts.end(); ++portIt) 
        {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket == -1) {
                std::cerr << "Error Socket : " << strerror(errno) << std::endl;
                continue;
            }
            int yes = 1;
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
                std::cerr << "Error Setsockopt : " << strerror(errno) << std::endl;
                close(serverSocket);
                continue;
            }

            struct sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_addr.s_addr = INADDR_ANY;
            int port = atoi(portIt->c_str());
            serverAddress.sin_port = htons(port);
            
            if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
            {
                std::cerr << "Error  bind : " << strerror(errno) << std::endl;
                close(serverSocket);
                continue;
            }
            cout << "Bind: " << serverSocket << " on port " << port << endl;
            if (listen(serverSocket, atoi(it->_maxClients.c_str())) == -1)
            {
                std::cerr << "Error listen : " << strerror(errno) << std::endl;
                close(serverSocket);
                continue;
            }
            cout << "Listening on http://" << it->_serverName << ":" << port << endl;
            socketToServerConfigMap[serverSocket] = *it;
            _fd.fd = serverSocket;
            _fd.events = POLLIN;
            listeningSocket.insert(serverSocket);
            fds.push_back(_fd);

            listeningPortMap[serverSocket] = port;
        }
    }
}

WebServer::~WebServer() 
{
    std::set<int>::iterator it;
    for (it = listeningSocket.begin(); it != listeningSocket.end(); ++it) {
        close(*it);
    }
    
}

void WebServer::start() 
{
    while (1) 
    {
        
        int result = poll(fds.data(), fds.size(), -1);
        if (result > 0)
        {
            checkTimeouts();
            for (size_t i = 0; i < fds.size(); ++i) 
            {
                if (fds[i].revents & POLLIN) 
                {
                    i = handle_incoming_connection(i);
                }
                if (fds[i].revents & POLLOUT)
                {
                    i = handle_outcoming_connection(i);
                }
            }
        }
    }
}


size_t WebServer::checkClientMaxBodySize(char *buffer, int bytesRead, size_t i)
{
    string request(buffer, bytesRead);
    map<string, string> headers = parseHeaders(request);
    string connectionHeader = headers.find("Connection") != headers.end() ? headers.at("Connection") : "";
    
    _request = request;
    if (connectionHeader == "keep-alive")
        _count = 1;
    else
        _count = 0;
    string cookieHeader = headers.find("Cookie") != headers.end() ? headers.at("Cookie") : "";
    _cookies = parseCookies(cookieHeader);

    int maxBodySize = atoi(socketToServerConfigMap[fds[i].fd]._clientMaxBodySize.c_str()) * 1000;
    size_t bodyPos = request.find("\r\n\r\n");
    std::string requestBody = (bodyPos != std::string::npos) ? request.substr(bodyPos + 4) : "";
    
    if (requestBody.size() > maxBodySize)
    {
        _response = sendError413(fds[i].fd);
        fds[i].events = POLLOUT;
    }
    else 
    {
        _response = handleRequest(fds[i].fd, request);
        fds[i].events = POLLOUT;
    }
    return i;
}

bool WebServer::isListeningSocket(int fd)                                                                                                                                                                                                                                                                                                                                                                                           
{
    return listeningSocket.find(fd) != listeningSocket.end();
}

bool isDirectory(const std::string& path) 
{
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}


std::string generateDirectoryListingHTML(const std::string& directoryPath) {
    DIR *dir;
    struct dirent *ent;
    std::ostringstream html;

    html << "<html><head><title>Index of " << directoryPath << "</title></head><body>";
    html << "<h1>Index of " << directoryPath << "</h1>";
    html << "<ul>";

    if ((dir = opendir(directoryPath.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL) {
            html << "<li><a href=\"" << ent->d_name << "\">" << ent->d_name << "</a></li>";
        }
        closedir(dir);
    }
    else
    {
        html << "<p>Error opening directory.</p>";
    }
    html << "</ul></body></html>";
    return html.str();
}

std::string sendInternalServerError(int clientSocket) {
    std::string response = "HTTP/1.1 500 Internal Server Error\r\n";
    response += "Content-Length: 0\r\n";
    response += "Connection: close\r\n\r\n";

    send(clientSocket, response.c_str(), response.size(), 0);

    return response;
}

std::string executeCGI(const std::string& scriptPath, const std::string& request, int clientSocket) {
    int pipefd[2];
    if (pipe(pipefd) == -1) 
    {
        std::cerr << "Error creating pipe: " << strerror(errno) << std::endl;
        return sendInternalServerError(clientSocket);
    }

    pid_t pid = fork();
    if (pid == -1) 
    {
        std::cerr << "Error forking process: " << strerror(errno) << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        return sendInternalServerError(clientSocket);
    }

    if (pid == 0) 
    { // Child process
        close(pipefd[0]); // Close read end of the pipe

        // Redirect stdout to the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Set up environment variables
        char* env[] = {NULL};  // You can customize the environment if needed

        // Execute the CGI script with the provided environment
        execle(scriptPath.c_str(), scriptPath.c_str(), NULL, env);

        // If execle fails
        std::cerr << "Error executing CGI script: " << strerror(errno) << std::endl;
        close(pipefd[1]);
        _exit(EXIT_FAILURE);
    } 
    else 
    { // Parent process
        close(pipefd[1]); // Close write end of the pipe

        // Read the output from the CGI script
        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer));
        close(pipefd[0]);

        if (bytesRead == -1) 
        {
            std::cerr << "Error reading from pipe: " << strerror(errno) << std::endl;
            return sendInternalServerError(clientSocket);
        }

        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) 
        {
            // CGI script executed successfully
            return std::string(buffer, bytesRead);
        } 
        else 
        {
            std::cerr << "CGI script failed to execute" << std::endl;
            return sendInternalServerError(clientSocket);
        }
    }
}

string WebServer::handleRequest(int clientSocket, const std::string& request) 
{
    std::string scriptName;
    std::istringstream requestStream(request);
    std::string method, path, version;
    vector<string>::iterator it;
    vector<string>::iterator it_2;
    requestStream >> method >> path >> version;
    int get = 0; int post = 0; int del = 0 ;
    int get_2 = 0; int post_2 = 0; int del_2 = 0 ;

    for (it = socketToServerConfigMap[clientSocket]._methods.begin(); it != socketToServerConfigMap[clientSocket]._methods.end(); ++it)
    {
        if (*it == "GET")
            get = 1;
        else if (*it == "POST")
            post = 1;
        else if (*it == "DELETE")
            del = 1;
    }
    if ((method == "GET" && get == 0) || (method == "POST" && post == 0) || (method == "DELETE" && del == 0))
    {
        cerr << "Method " << method << " not allowed" << endl;
        return sendError405();
    }
    for (it_2 = socketToServerConfigMap[clientSocket]._allow_methods.begin(); it_2 != socketToServerConfigMap[clientSocket]._allow_methods.end(); ++it_2)
    {
        if (*it_2 == "GET")
            get_2 = 1;
        else if (*it_2 == "POST")
            post_2 = 1;
        else if (*it_2 == "DELETE")
            del_2 = 1;
    }
    if ((method == "GET" && get_2 == 0 && path == "/upload") || (method == "POST" && post_2 == 0 && path == "/upload") || (method == "DELETE" && del_2 == 0 && path == "/upload"))
    {
        cerr << "Method " << method << " not allowed" << endl;
        return sendError405();
    }
    //if (method != "GET" && method != "POST" && method != "DELETE") {
    //     cout << "DEBUG_01\n";
    //    return sendBadRequestResponse(clientSocket);
    //}
    if (path.find("..") != std::string::npos) 
    {
         cout << "DEBUG_00\n";
        return sendBadRequestResponse(clientSocket);
    }
    if (path.find(".css") != std::string::npos)
    {
        return sendCssResponse(clientSocket);
    }
    if (version != "HTTP/1.1") {
        return sendError505(clientSocket);
    }
    if (method == "GET") 
    {
        if (path == "/cgi-bin/script3.py")
            return executeCGI("./cgi-bin/script3.py", request, clientSocket);
        else if (path == "/")
            return sendFileResponse(clientSocket, socketToServerConfigMap[clientSocket]._index);
        else if (path == "/test_curl.txt")
            return sendError403(clientSocket);
        else if (path == "/upload_checker")
            return sendFileResponse(clientSocket, "html/data/upload_checker.html");
        else if (path == "/login")
            return sendFileResponse(clientSocket, "html/data/login.html");
        else if (path=="/home")
            return sendSessionResponse(clientSocket, "html/data/home.html");
        else if (path=="/logout")
            return sendFileResponse(clientSocket, "html/data/logout.html");
        else
        {
            if (isDirectory(path.substr(1)) && socketToServerConfigMap[clientSocket]._autoindex == "off")
                return sendFileResponse(clientSocket, socketToServerConfigMap[clientSocket]._index);
            else if (isDirectory(path.substr(1)) && socketToServerConfigMap[clientSocket]._autoindex == "on")
                return sendHttpResponse(clientSocket, generateDirectoryListingHTML(path.substr(1)));
            else
                return sendFileResponse(clientSocket, path.substr(1));
        }
    }
    else if (method == "POST") 
    {
        if (path == "/upload")
           return handleFileUpload(clientSocket, requestStream);
        else if (path=="/login")
        {
           return handleLogin(clientSocket, requestStream);
        }
        else if (path=="/logout")
        {
            return handleLogout(clientSocket);
        }
        else
           return sendTextResponse(clientSocket, "HTTP/1.1 200 OK\n");
    } 
    else if (method == "DELETE") 
    {
        if (path == "/delete")
            return sendTextResponse(clientSocket, "File deleted successfully");
        else
        {    
            return sendNotFoundResponse(clientSocket);
        }
    } 
    else
        return sendBadRequestResponse(clientSocket);
    return ("");
}