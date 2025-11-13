#include "ft_malloc.h"
#include <stdlib.h>

t_debug_config g_debug_config = {false, false, false, false, -1, 0, 0, 0};

static int ft_atoi(const char *str)
{
	int result = 0;
	int sign = 1;
	
	if (!str)
		return 0;
	
	while (*str == ' ' || *str == '\t')
		str++;
	
	if (*str == '-') {
		sign = -1;
		str++;
	} else if (*str == '+') {
		str++;
	}
	
	while (*str >= '0' && *str <= '9') {
		result = result * 10 + (*str - '0');
		str++;
	}
	
	return result * sign;
}

void init_debug_config(void)
{
	char *env;
	
	if (g_debug_config.initialized)
		return;
	
	env = getenv("FT_MALLOC_DEBUG");
	if (env && ft_atoi(env) != 0)
		g_debug_config.debug = true;
	
	env = getenv("FT_MALLOC_STATS");
	if (env && ft_atoi(env) != 0)
		g_debug_config.stats = true;
	
	env = getenv("FT_MALLOC_CHECK");
	if (env && ft_atoi(env) != 0)
		g_debug_config.check = true;
	
	env = getenv("FT_MALLOC_PERTURB");
	if (env) {
		int value = ft_atoi(env);
		if (value >= 0 && value <= 255)
			g_debug_config.perturb = value;
	}
	
	g_debug_config.initialized = true;
	
}

void debug_log_malloc(void *ptr, size_t size)
{
	if (!g_debug_config.debug)
		return;
	
	write_color(2, COLOR_GREEN);
	write_str(2, "[MALLOC] ");
	write_color(2, COLOR_RESET);
	write_str(2, "size=");
	write_number(2, size);
	write_str(2, " -> ");
	write_hex(2, ptr);
	write_str(2, "\n");
}

void debug_log_free(void *ptr)
{
	if (!g_debug_config.debug)
		return;
	
	write_color(2, COLOR_RED);
	write_str(2, "[FREE] ");
	write_color(2, COLOR_RESET);
	write_hex(2, ptr);
	write_str(2, "\n");
}

void debug_log_realloc(void *old_ptr, void *new_ptr, size_t size)
{
	if (!g_debug_config.debug)
		return;
	
	write_color(2, COLOR_YELLOW);
	write_str(2, "[REALLOC] ");
	write_color(2, COLOR_RESET);
	write_hex(2, old_ptr);
	write_str(2, " -> ");
	write_hex(2, new_ptr);
	write_str(2, " size=");
	write_number(2, size);
	write_str(2, "\n");
}

void debug_perturb_alloc(void *ptr, size_t size)
{
	if (g_debug_config.perturb < 0)
		return;
	
	unsigned char *mem = (unsigned char *)ptr;
	unsigned char pattern = (unsigned char)g_debug_config.perturb;
	
	for (size_t i = 0; i < size; i++)
		mem[i] = pattern;
}

void debug_perturb_free(void *ptr, size_t size)
{
	if (g_debug_config.perturb < 0)
		return;
	
	unsigned char *mem = (unsigned char *)ptr;
	unsigned char pattern = (unsigned char)(~g_debug_config.perturb);
	
	for (size_t i = 0; i < size; i++)
		mem[i] = pattern;
}

bool debug_check_double_free(t_block *block)
{
	if (!g_debug_config.check)
		return false;
	
	if (block->free) {
		debug_free_invalid_pointer((void *)block->data);
		return true;
	}
	
	return false;
}

void debug_free_invalid_pointer(void *ptr)
{
	write_color(2, COLOR_RED);
	write_color(2, COLOR_BOLD);
	write_str(2, "[ERROR] Double-free detected at ");
	write_hex(2, ptr);
	write_str(2, "\n");
	write_color(2, COLOR_RESET);
}

void debug_show_stats(void)
{
	if (!g_debug_config.stats && !g_debug_config.initialized)
		return;
	
	write_color(2, COLOR_CYAN);
	write_color(2, COLOR_BOLD);
	write_str(2, "\n=== MALLOC STATISTICS ===\n");
	write_color(2, COLOR_RESET);
	
	write_str(2, "Total malloc calls: ");
	write_number(2, g_debug_config.total_mallocs);
	write_str(2, "\n");
	
	write_str(2, "Total free calls: ");
	write_number(2, g_debug_config.total_frees);
	write_str(2, "\n");
	
	write_str(2, "Total realloc calls: ");
	write_number(2, g_debug_config.total_reallocs);
	write_str(2, "\n");
	
	size_t leaks = g_debug_config.total_mallocs - g_debug_config.total_frees;
	if (leaks > 0) {
		write_color(2, COLOR_RED);
		write_str(2, "Potential memory leaks: ");
		write_number(2, leaks);
		write_str(2, " allocations not freed\n");
		write_color(2, COLOR_RESET);
	} else {
		write_color(2, COLOR_GREEN);
		write_str(2, "No memory leaks detected\n");
		write_color(2, COLOR_RESET);
	}
	
	write_str(2, "\n");
}

__attribute__((destructor))
static void debug_exit_handler(void)
{
	if (g_debug_config.stats)
		debug_show_stats();
}
