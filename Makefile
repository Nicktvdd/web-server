NAME = server

CC = g++
CFLAGS = -Wall -Wextra -Werror -g 
RM = rm -rf

# INCLUDES = -I includes -I libft/includes

HEADERS	= incl/server.hpp incl/socket.hpp incl/manager.hpp

SRCS = src/server.cpp \
src/socket.cpp main.cpp src/manager/manager.cpp \
src/manager/Cgi.cpp src/manager/Request.cpp src/manager/Communication.cpp \
src/manager/Poll.cpp src/manager/ReadConfig.cpp

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