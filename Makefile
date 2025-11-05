ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
BIN_NAME = test_malloc
SYMLINK_NAME = libft_malloc.so
BIN_TO_RUN = ./test_malloc
CURR_DIR = $(shell pwd)
PROG = 
DFLAGS =

SRCS = $(wildcard srcs/*.c)
OBJS = $(addprefix $(OBJS_DIR),$(SRCS:$(SRCS_DIR)%.c=%.o))
SRCS_DIR = srcs/
OBJS_DIR = objs/
HEADERS = -I includes/headers/
HDRS = $(wildcard includes/headers/*.h)

RM = rm -f
CFLAGS = -Wall -Wextra -Werror -g -fPIC
SFLAGS = -fsanitize=address
VFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes
CC = cc

#color codes
GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
ORANGE = \033[0;33m
NC = \033[0m

all: $(NAME) $(SYMLINK_NAME)

debug: CFLAGS += -DDEBUG_MALLOC
debug: re

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(HDRS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

$(NAME): $(OBJS)
	@echo "$(GREEN)$(NAME)$(NC) compiling..."
	@$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJS) 
	@echo "$(GREEN)$(NAME)$(NC) ready!"

$(SYMLINK_NAME): $(NAME)
	@ln -sf $(NAME) $(SYMLINK_NAME)
	@echo "$(BLUE)Created symlink: $(SYMLINK_NAME) -> $(NAME)$(NC)"

clean:
	@$(RM) -r $(OBJS_DIR)
	@$(RM) -r $(OBJS_DIR_BONUS)
	@$(RM) -r $(OBJS_DIR_S)
	@echo "$(RED)$(NAME)$(NC) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(SYMLINK_NAME)
	@$(RM) $(BIN_NAME)
	@echo "$(RED)$(NAME)$(NC) cleaned!"

v: all
	@$(CC) $(CFLAGS) $(HEADERS) main.c -L. -lft_malloc_$(HOSTTYPE) -Wl,-rpath=. -o $(BIN_NAME)
	@echo "$(GREEN)Running test with valgrind and custom malloc...$(NC)"
	@LD_LIBRARY_PATH=. valgrind $(VFLAGS) $(BIN_TO_RUN)

fcount:
	@echo "you wrote $(RED)$(shell cat $(SRCS) | wc -l)$(NC)lines of code"

test_preload: all
	@echo "$(GREEN)Loading custom malloc into: $(PROG)$(NC)"
	@LD_PRELOAD=$(CURR_DIR)/$(SYMLINK_NAME) $(DFLAGS) $(PROG)

re: fclean all

.PHONY: all fclean clean re v s fcount send run test test_preload