#include "ft_malloc.h"
#include <stdlib.h>

void cleanup_check(void) {
	write(1, "\n=== Memory state at program exit ===\n", 39);
	show_alloc_mem();
}

int main() {
	char *str;
	atexit(cleanup_check);

	str = strdup("Hello, World! This is a test string for malloc and realloc.");
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
	void *ptr1 = calloc(5000, sizeof(char));
	printf("Allocated 5000 bytes at %p\n", ptr1);
	free(ptr1);
	free(str);
	return 0;
}
