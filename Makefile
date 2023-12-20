NAME = webserv
NAME_DEBUG = web_debug

VPATH = src/ src/http_request/ src/http_response/ src/server_engine/ src/config/

SRCS =	main.cpp \
		request.cpp \
		response.cpp \
		directory_autoindex.cpp \
		handle_request_by_method.cpp \
		set_root_index_path.cpp \
		context.cpp \
		config_parser.cpp \
		config_tools.cpp \
		server.cpp \
		cgi.cpp \
		status_code.cpp \
		socket.cpp

OBJDIR = .obj
OBJDIR_DEBUG = .obj_debug
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
OBJS_DEBUG = $(SRCS:%.cpp=$(OBJDIR_DEBUG)/%.o)

RM = rm -rf

CC = c++
CXX_FLAGS = -Wextra -Wall -Werror -std=c++98
HEAD = -I includes

${OBJDIR}/%.o : %.cpp | $(OBJDIR)
	${CC} ${CXX_FLAGS} ${HEAD} -c $< -o $@

${OBJDIR_DEBUG}/%.o : %.cpp | $(OBJDIR_DEBUG)
	${CC} ${CXX_FLAGS} -D DEBUG -g3 ${HEAD} -c $< -o $@

$(NAME) : ${OBJS}
	${CC} ${CXX_FLAGS} ${OBJS} -o ${NAME}

$(NAME_DEBUG) : ${OBJS_DEBUG}
	${CC} ${CXX_FLAGS} -D DEBUG -g3 ${OBJS_DEBUG} -o ${NAME}

$(OBJDIR) :
	mkdir -p $@
$(OBJDIR_DEBUG) :
	mkdir -p $@

all : ${NAME}

debug : ${NAME_DEBUG} 

clean :
	${RM} ${OBJDIR} ${OBJDIR_DEBUG}

fclean : clean
	${RM} ${NAME} ${NAME_DEBUG}

re : fclean all

dre : fclean debug

.PHONY : all clean fclean re debug
