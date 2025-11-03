#include "ft_malloc.h"

int main() {
	void *ptr = malloc(1000);
	if (ptr == NULL) {
		write(2, "malloc failed\n", 14);
		return 1;
	}
	char *str = ft_strdup("Hello, custom malloc!");
	ft_printf("%s\n", str);
	void *ptr2 = malloc(4912);
	if (ptr2 == NULL) {
		write(2, "malloc failed\n", 14);
		return 1;
	}
	// show_alloc_mem();
	free(ptr);
	free(str);
	free(ptr2);
	free(NULL);
	show_alloc_mem();
	return 0;
}
