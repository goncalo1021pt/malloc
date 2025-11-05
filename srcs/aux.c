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

void    *ft_memcpy(void *dest, const void *src, size_t n)
{
        unsigned char           *mem;
        const unsigned char     *old;
        size_t                          ctd;

        ctd = 0;
        mem = dest;
        old = src;
        while (ctd < n && (src || dest))
        {
                mem[ctd] = old[ctd];
                ctd++;
        }
        return (dest);
}

void write_color(int fd, const char *color)
{
	write_str(fd, color);
}

int is_printable(unsigned char c)
{
	return (c >= 32 && c <= 126);
}

void write_byte_hex(int fd, unsigned char byte)
{
	char hex[] = "0123456789abcdef";
	char buffer[3];
	
	buffer[0] = hex[byte >> 4];
	buffer[1] = hex[byte & 0x0F];
	buffer[2] = '\0';
	write(fd, buffer, 2);
}

void print_ascii(int fd, unsigned char *data, size_t len)
{
	size_t i;
	
	write_str(fd, "|");
	for (i = 0; i < 16; i++) {
		if (i < len) {
			if (is_printable(data[i]))
				write(fd, (char *)&data[i], 1);
			else
				write_str(fd, ".");
		} else {
			write_str(fd, " ");
		}
	}
	write_str(fd, "|");
}

void print_hexdump(int fd, void *ptr, size_t size, size_t max_bytes)
{
	unsigned char *data = (unsigned char *)ptr;
	size_t offset = 0;
	size_t bytes_to_dump = size > max_bytes ? max_bytes : size;
	size_t i;
	
	while (offset < bytes_to_dump) {
		write_str(fd, "    ");
		write_hex(fd, (void *)((unsigned long)data + offset));
		write_str(fd, ": ");
		
		for (i = 0; i < 16; i++) {
			if (offset + i < bytes_to_dump) {
				write_byte_hex(fd, data[offset + i]);
				write_str(fd, " ");
			} else {
				write_str(fd, "   ");
			}
			
			if (i == 7)
				write_str(fd, " ");
		}
		
		write_str(fd, " ");
		print_ascii(fd, data + offset, bytes_to_dump - offset);
		write_str(fd, "\n");
		
		offset += 16;
	}
	
	if (size > max_bytes) {
		write_str(fd, "    ... (");
		write_number(fd, size - max_bytes);
		write_str(fd, " more bytes)\n");
	}
}
