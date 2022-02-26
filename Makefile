NAME = webserv

CXX = clang++

# SANFLAGS = -fsanitize=address

SANFLAGS = -fsanitize=leak

DEBUGFLAGS = -g -fno-limit-debug-info -DDEBUG

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(DEBUGFLAGS) $(SANFLAGS)

CPPFLAGS += -MMD -MP

SRC = $(wildcard ./src/*.cpp)
SRC += $(wildcard ./src/*/*.cpp)

INCLUDES = ./includes/ -I src/Autoindex -I src/config -I src/Client -I src/Server -I src/CGI

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(OBJ): %.o: %.cpp $(SRC) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -I $(INCLUDES) -o $@


$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)
	$(RM) $(SRC:.cpp=.d)

fclean: clean
	$(RM) $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

run: $(NAME)
	# ASAN_OPTIONS=alloc_dealloc_mismatch=0 ASAN_OPTIONS=detect_leaks=0 ./$(NAME) $(filter-out $@,$(MAKECMDGOALS))
	ASAN_OPTIONS=alloc_dealloc_mismatch=0 ./$(NAME) $(filter-out $@,$(MAKECMDGOALS))

-include $(OBJ:%.o=%.d)
	
.PHONY: all clean fclean re
