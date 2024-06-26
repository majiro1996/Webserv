# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cmorales <moralesrojascr@gmail.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/22 11:35:35 by manujime          #+#    #+#              #
#    Updated: 2024/04/22 12:37:58 by cmorales         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -g -Wall -Wextra -Werror -std=c++98
SRC_DIR = ./sources
INC_DIR = ./includes
OBJ_DIR = ./obj
NAME = webserv

GREEN		=		\033[0;32m
RED			=		\033[0;31m
END			=		\033[0m

SRC = $(wildcard $(SRC_DIR)/*.cpp)

SRC_EXCLUDE_TCPCLIENT = $(filter-out $(SRC_DIR)/tcpclient.cpp $(SRC_DIR)/test.cpp, $(SRC))

OBJ = $(SRC_EXCLUDE_TCPCLIENT:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled$(END)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

bonus: $(BONUS)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)$(NAME) objects deleted$(END)"

fclean: clean
	@rm -f $(NAME)
	@rm -rf a.out
	@echo "$(RED)$(NAME) deleted$(END)"

re: fclean all

r: re
	@./$(NAME) conf/server.conf

l: re
	@./$(NAME) conf/server_linux.conf

val: re
	@valgrind --leak-check=full ./$(NAME) conf/server_linux.conf

.PHONY: all clean fclean re