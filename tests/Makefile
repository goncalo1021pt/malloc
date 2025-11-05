CC = cc
CFLAGS = -Wall -Wextra -Werror -I../includes/headers
LDFLAGS = -L.. -lft_malloc -Wl,-rpath,..
LIB = ../libft_malloc_x86_64.so

TESTS = test_perturb test_debug test_advanced test_comprehensive

all: $(TESTS)

$(TESTS): %: %.c $(LIB)
	@$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

run_perturb: test_perturb
	@echo "=== Running Perturb Test ==="
	LD_PRELOAD=$(LIB) FT_MALLOC_PERTURB=170 ./test_perturb

run_debug: test_debug
	@echo "=== Running Debug Test (with logging) ==="
	LD_PRELOAD=$(LIB) FT_MALLOC_DEBUG=1 ./test_debug 2>&1 | head -50
	@echo "\n=== Running Debug Test (with stats) ==="
	LD_PRELOAD=$(LIB) FT_MALLOC_STATS=1 ./test_debug

run_advanced: test_advanced
	@echo "=== Running Advanced Test ==="
	LD_PRELOAD=$(LIB) FT_MALLOC_CHECK=1 ./test_advanced

run_comprehensive: test_comprehensive
	@echo "=== Running Comprehensive Test ==="
	LD_PRELOAD=$(LIB) \
		FT_MALLOC_DEBUG=1 \
		FT_MALLOC_STATS=1 \
		FT_MALLOC_CHECK=1 \
		FT_MALLOC_PERTURB=170 \
		./test_comprehensive 2>&1 | tail -100

run_all: run_perturb run_debug run_advanced run_comprehensive

clean:
	@rm -f $(TESTS)

.PHONY: all clean run_perturb run_debug run_advanced run_comprehensive run_all
