/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 14:25:47 by fbardeau          #+#    #+#             */
/*   Updated: 2024/02/16 13:10:31 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cstdlib>
#include <set>
#include <map>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <dirent.h>
#include <fcntl.h>


using namespace std;


std::string trim(const std::string& str);
std::map<std::string, std::string> parseCookies(const std::string& cookieHeader);
std::map<std::string, std::string> parseHeaders(const std::string& request);

#endif