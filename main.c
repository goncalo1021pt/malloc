#include "ft_malloc.h"

int main() {
	void *ptr = malloc(1000);
	if (ptr == NULL) {
		write(2, "malloc failed\n", 14);
		return 1;
	}
	char *str = ft_strdup("Hello, custom malloc!");
	ft_printf("%s\n", str);
	char *str2 = ft_strtrim(str, "Hd!");
	ft_printf("Trimmed string: '%s'\n", str2);
	show_alloc_mem();
	free(ptr);
	free(str);
	free(str2);
	free(NULL);
	show_alloc_mem();
	return 0;
}
