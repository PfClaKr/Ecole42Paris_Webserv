NAME = webserv

VPATH = http_request/ http_response/ server_engine/ config/

SRCS =	main.cpp \
		request.cpp \
		response.cpp \
		config.cpp \
		server.cpp \
		socket.cpp

OBJDIR = .obj
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

RM = rm -rf

CC = c++
CXX_FLAGS = -g3 #-Wextra -Wall -Werror
HEAD = -I includes

${OBJDIR}/%.o : %.cpp | $(OBJDIR)
	${CC} ${CXX_FLAGS} ${HEAD} -c $< -o $@

$(NAME) : ${OBJS}
	${CC} ${CXX_FLAGS} ${OBJS} -o ${NAME}

$(OBJDIR) :
	mkdir -p $@

all : ${NAME}

clean :
	${RM} ${OBJDIR}

fclean : clean
	${RM} ${NAME}

re : fclean all

.PHONY : all clean fclean re