NAME = webserv

CXX = clang++

SANFLAGS = -fsanitize=address
# SANFLAGS = -fsanitize=leak

CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98 $(SANFLAGS)

SRC = $(wildcard ./src/*.cpp)

HEADERS = $(wildcard ./includes/*.hpp)

INCLUDES = .

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(OBJ): %.o: %.cpp $(SRC) $(HEADERS) Makefile
	$(CXX) $(CXXFLAGS) -c $< -I $(INCLUDES) -o $@

$(NAME): $(OBJ) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

run: $(NAME)
	./$(NAME)
	
.PHONY: all clean fclean re
