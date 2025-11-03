#include "ft_malloc.h"

size_t ft_strlen_local(const char *s)
{
	size_t len = 0;
	while (s[len])
		len++;
	return len;
}

void write_str(int fd, const char *str)
{
	write(fd, str, ft_strlen_local(str));
}

void write_hex(int fd, void *ptr)
{
	char hex[] = "0123456789abcdef";
	char buffer[20];
	unsigned long addr = (unsigned long)ptr;
	int i = 17;
	
	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[18] = '\0';
	
	if (addr == 0) {
		write(fd, "0x0", 3);
		return;
	}
	
	while (i >= 2) {
		buffer[i] = hex[addr & 0xF];
		addr >>= 4;
		i--;
	}
	
	i = 2;
	while (i < 18 && buffer[i] == '0')
		i++;
	
	write(fd, buffer + i, 18 - i);
}

void write_number(int fd, size_t num)
{
	char buffer[32];
	int i = 30;
	
	buffer[31] = '\0';
	
	if (num == 0) {
		write(fd, "0", 1);
		return;
	}
	
	while (num > 0) {
		buffer[i] = '0' + (num % 10);
		num /= 10;
		i--;
	}
	
	write(fd, buffer + i + 1, 30 - i);
}