# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fbardeau <fbardeau@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/08 13:14:05 by vkuzmin           #+#    #+#              #
#    Updated: 2024/02/21 10:49:13 by fbardeau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = g++
CFLAGS = --std=c++98 -fsanitize=address
TARGET = webserv
SRCS =	srcs/main.cpp \
		srcs/WebServer.cpp \
		srcs/load_config.cpp \
		srcs/upload.cpp \
		srcs/Response.cpp \
		srcs/utils.cpp \
		srcs/Session_cookie.cpp \
		srcs/request.cpp
		
OBJS = $(SRCS:.cpp=.o) 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re