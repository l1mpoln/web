/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 22:26:40 by vkuzmin           #+#    #+#             */
/*   Updated: 2024/02/15 17:53:45 by fbardeau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WebServer.hpp"

int main(int argc, char** argv) 
{
    if (argc != 2)
        return (cout << "not enought argument" << endl, 0);
    WebServer server(argv[1]);
    server.start();

    return 0;
}
