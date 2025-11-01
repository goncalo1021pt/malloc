#include "ft_malloc.h"

int main() {
	void *ptr = malloc(1000);
	if (ptr == NULL) {
		write(2, "malloc failed\n", 14);
		return 1;
	}
	ft_printf("Allocated 1000 bytes at %p\n", ptr);
	free(ptr);
	return 0;
}
