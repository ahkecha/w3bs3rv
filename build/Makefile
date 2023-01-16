CXX = g++
CXXFLAGS = -Wall -Wextra -Werror

OBJECT_PATH = ../object_files/
FOBJ_SRC = ../object_files
SRC = main.cpp Socket.cpp Server.cpp WebServ.cpp Request.cpp Config.cpp Lexer.cpp CGIx.cpp

SRC_PATH = ../src/

NAME = ../webserv

INCLUDE = $(addprefix ../include/, webserv.hpp Request.hpp Server.hpp Socket.hpp Config.hpp Lexer.hpp struct.hpp errors.hpp headers.hpp CGIx.hpp MimeTypes.hpp)
OBJECTS = $(addprefix $(OBJECT_PATH), $(SRC:.cpp=.o))

all: $(NAME) clean

$(NAME): $(FOBJ_SRC) $(OBJECTS) $(INCLUDE)
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@


$(OBJECT_PATH)%.o : $(SRC_PATH)%.cpp $(INCLUDE)
	@$(CXX) $(CXXFLAGS) -o $@  -c $<


$(FOBJ_SRC) :
	@mkdir $@

clean:
	@rm -rf $(OBJECTS)
	@rm -rf $(FOBJ_SRC)
	@rm -rf ../config.log ../config.status

fclean: clean
	@rm -rf $(NAME)

re: fclean all
