#include "ft_malloc.h"

void cleanup_check(void) {
	write(1, "\n=== Memory state at program exit ===\n", 39);
	show_alloc_mem();
}

int main() {
	char *str;
	atexit(cleanup_check);

	str = ft_strdup("Hello, World!");
	if (!str) {
		write(2, "strdup failed\n", 15);
		return 1;
	}
	printf("%s\n", str);
	str = realloc(str, 51);
	if (!str) {
		write(2, "realloc failed\n", 16);
		return 1;
	}
	int ctd = 0;
	while (ctd < 50) {
		str[ctd] = 'A';
		ctd++;
	}
	str[ctd] = '\0';
	printf("%s\n", str);
	free(str);
	show_alloc_mem();
	return 0;
}
