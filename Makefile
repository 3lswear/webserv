NAME = webserv

CXX = clang++

SANFLAGS = -fsanitize=address

# SANFLAGS = -fsanitize=leak

CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98 $(SANFLAGS) -DDEBUG

CPPFLAGS += -MD -MP

SRC = $(wildcard ./src/*.cpp)
SRC += $(wildcard ./src/*/*.cpp)

INCLUDES = ./includes/ -I src/Autoindex -I src/config -I src/Client -I src/Server

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(OBJ): %.o: %.cpp $(SRC) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -I $(INCLUDES)  -o $@

-include $(SRC:%.cpp=%.d)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ)  -o $(NAME)

clean:
	$(RM) $(OBJ)
	$(RM) $(SRC:.cpp=.d)

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

run: $(NAME)
	ASAN_OPTIONS=detect_leaks=0 ./$(NAME)
	
.PHONY: all clean fclean re
