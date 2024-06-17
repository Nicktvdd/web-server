NAME = server

CC = g++
CFLAGS = -Wall -Wextra -Werror -g 
RM = rm -rf

# INCLUDES = -I includes -I libft/includes

HEADERS	= incl/server.hpp incl/socket.hpp incl/Manager.hpp

SRCS = src/server.cpp \
src/socket.cpp main.cpp src/Manager/Manager.cpp \
src/Manager/Cgi.cpp src/Manager/Request.cpp src/Manager/Communication.cpp \
src/Manager/Poll.cpp src/Manager/ReadConfig.cpp

OBJS = $(SRCS:.cpp=.o)

all:
	@$(MAKE) $(NAME) -j5
$(NAME) : $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: 	fclean all

.PHONY: all clean fclean re