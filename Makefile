NAME = IRC
CXX = c++
CXXFLAGS = -std=c++98 -g

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=log_valgrind.txt

SRCS =	main.cpp Server.cpp User.cpp Channel.cpp CommandHandler.cpp
HEADERS = Server.hpp User.hpp Channel.hpp CommandHandler.hpp

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	@echo "Compiling executable..."
	@$(CXX) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean:
	@echo "Nothing to clean..."

fclean: clean
	@echo "Removing executable..."
	@rm -rf $(NAME)

re: fclean all


# valgrind ./IRC <port> <password>

val: $(NAME)
	@echo "Running Valgrind..."
	@$(VALGRIND) ./$(NAME) $(port) $(password)

.PHONY: all clean fclean re debug
.SILENT:
