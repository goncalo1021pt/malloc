ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
BIN_NAME = test_malloc
SYMLINK_NAME = libft_malloc.so
BIN_TO_RUN = ./test_malloc

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

# libft
LIBFT_DIR = ./includes/libft
LIBFT = $(LIBFT_DIR)/libft.a

#color codes
GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
ORANGE = \033[0;33m
NC = \033[0m

all: $(NAME) $(SYMLINK_NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c $(HDRS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(HEADERS) -c $< -o $@

$(OBJS_DIR_S)/%.o: srcs/%.c $(HDRS)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SFLAGS) $(HEADERS) -c $< -o $@

$(NAME): $(OBJS) $(LIBFT)
	@echo "$(GREEN)$(NAME)$(NC) compiling..."
	@$(CC) $(CFLAGS) -shared -o $(NAME) $(OBJS) $(LIBFT)
	@echo "$(GREEN)$(NAME)$(NC) ready!"

$(SYMLINK_NAME): $(NAME)
	@ln -sf $(NAME) $(SYMLINK_NAME)
	@echo "$(BLUE)Created symlink: $(SYMLINK_NAME) -> $(NAME)$(NC)"

$(BONUS_NAME): $(BONUS_OBJS) $(LIBFT)
	@$(CC) $(CFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS) $(LIBFT)
	@echo $(BONUS_NAME)ready!

$(LIBFT):
	@echo "$(ORANGE)libft$(NC) compiling..."
	@$(MAKE) -C $(LIBFT_DIR) --no-print-directory
	@echo "$(ORANGE)libft$(NC) ready!"

s: fclean $(SOBJS) $(LIBFT)
	@echo "$(GREEN)$(NAME)$(NC) compiling with $(ORANGE)$(SFLAGS)...$(NC)"
	@$(CC) $(CFLAGS) $(SFLAGS) -o $(NAME) $(SOBJS) $(LIBFT)

clean:
	@$(RM) -r $(OBJS_DIR)
	@$(RM) -r $(OBJS_DIR_BONUS)
	@$(RM) -r $(OBJS_DIR_S)
	@$(MAKE) -C $(LIBFT_DIR) clean --no-print-directory
	@echo "$(RED)$(NAME)$(NC) OBJS cleaned!"

fclean: clean
	@$(RM) $(NAME)
	@$(RM) $(SYMLINK_NAME)
	@$(RM) $(BONUS_NAME)
	@$(RM) $(BIN_NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean --no-print-directory
	@echo "$(RED)$(NAME)$(NC) cleaned!"

v: all
	@$(CC) $(CFLAGS) $(HEADERS) main.c -L. -lft_malloc_$(HOSTTYPE) -Wl,-rpath=. $(LIBFT) -o $(BIN_NAME)
	@echo "$(GREEN)Running test with valgrind and custom malloc...$(NC)"
	@LD_LIBRARY_PATH=. valgrind $(VFLAGS) ./$(BIN_TO_RUN)

fcount:
	@echo "you wrote $(RED)$(shell cat $(SRCS) | wc -l)$(NC)lines of code"

send: fclean
	git add . && git commit -m "auto" && git push

run: all
	@$(CC) $(CFLAGS) $(HEADERS) main.c $(LIBFT) -L. -lft_malloc_$(HOSTTYPE) -Wl,-rpath=. -o $(BIN_NAME)
	@echo "$(GREEN)Running test with custom malloc...$(NC)"
	@./$(BIN_NAME)

test_preload: all
	@echo "$(GREEN)Loading custom malloc into: $(PROG)$(NC)"
	@LD_PRELOAD=./$(SYMLINK_NAME) $(PROG)

re: fclean all

.PHONY: all fclean clean re v s fcount send run test test_preload