NAME = IRC
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes --log-file=log_valgrind.txt

SRCS_DIR = ./srcs/
COMMANDS_DIR = $(SRCS_DIR)commands/
HEADERS_DIR = ./includes/

SRCS =	main.cpp \
		$(addprefix $(SRCS_DIR), Server.cpp User.cpp Channel.cpp CommandHandler.cpp ReplyHandler.cpp utils.cpp) \
		$(addprefix $(COMMANDS_DIR), pass.cpp nick.cpp user.cpp invite.cpp join.cpp kick.cpp privmsg.cpp topic.cpp mode.cpp part.cpp quit.cpp bot.cpp ping.cpp)
HEADERS = $(addprefix $(HEADERS_DIR), Server.hpp User.hpp Channel.hpp CommandHandler.hpp ReplyHandler.hpp)

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	@echo "Compiling executable..."
	@$(CXX) $(CXXFLAGS) $(SRCS) -I$(HEADERS_DIR) -o $(NAME)

clean:
	@echo "Nothing to clean..."

fclean: clean
	@echo "Removing executable..."
	@rm -rf $(NAME)

re: fclean all

# make val port='1234' pass='xxxx' -> valgrind ./IRC 1234 xxxx
val: $(NAME)
	@echo "Running Valgrind..."
	@$(VALGRIND) ./$(NAME) $(port) $(pass)

.PHONY: all clean fclean re debug
.SILENT:
