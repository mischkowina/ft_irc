# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pdolinar <pdolinar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/02 12:40:43 by pdolinar          #+#    #+#              #
#    Updated: 2023/04/12 23:11:36 by pdolinar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	:=		ircserv
INC		:=		./include
HEADER	:=		$(INC)/server.hpp \
				$(INC)/client.hpp \
				$(INC)/channel.hpp \
				$(INC)/message.hpp \
				$(INC)/irc.hpp \
				$(INC)/error.hpp \
				$(INC)/cmds.hpp \
				$(INC)/reply.hpp \
				$(INC)/horoscope.hpp

CC			:=	c++
CPP_FLAGS	:=	-Wall -Wextra -Werror -std=c++98 -g -Wfatal-errors
RM			:=	rm -rf

SRC			:=	main.cpp \
				client.cpp \
				server.cpp \
				message.cpp \
				channel.cpp \
				utils.cpp \
				cmds_invite.cpp \
				cmds_join.cpp \
				cmds_kick.cpp \
				cmds_list.cpp \
				cmds_message.cpp \
				cmds_misc.cpp \
				cmds_mode.cpp \
				cmds_names.cpp \
				cmds_optional.cpp \
				cmds_part.cpp \
				cmds_server.cpp \
				cmds_topic.cpp \
				cmds_connection.cpp \
				cmds_message.cpp \
				cmds_misc.cpp \
				cmds_server.cpp \
				cmds_optional.cpp

SRC_DIR		:=	srcs/
OBJ_DIR		:=	build/

OBJ = $(addprefix $(OBJ_DIR),$(notdir $(SRC:.cpp=.o)))

all: $(OBJ_DIR) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CPP_FLAGS) -o $@ $^
	@echo "$(GR) > Compiled & linked!			\n\
	_______________________________________________ \n\
	    _____  ______          __   ____       __   \n\
	    /    '   /             /    /    )   /    ) \n\
	---/__------/-------------/----/___ /---/------ \n\
	  /        /             /    /    |   /        \n\
	_/________/___________ _/_ __/_____|__(____/___ \n\
                ------                         		\n $(CLEAR)"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp $(HEADER)
	$(CC) $(CPP_FLAGS) -I$(INC) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(PR)All object files deleted$(CLEAR)"

fclean:	clean
	@$(RM) $(NAME)
	@echo "$(PR)$(NAME) deleted$(CLEAR)"

CLEAR = \033[0m
PR = \033[1;35m
GR = \033[1;32m

re:	fclean all

.PHONY:	all clean fclean re
