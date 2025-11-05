# ft_malloc - Custom Memory Allocator

A high-performance, thread-safe memory allocator implementation in C that serves as a drop-in replacement for the standard library's `malloc`, `free`, `realloc`, and `calloc` functions. This project was developed as part of the 42 School curriculum and includes advanced debugging features and optimizations.

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![42 School](https://img.shields.io/badge/42-School-000000?logo=42)](https://42.fr)

## 📋 Table of Contents

- [Features](#-features)
- [How It Works](#-how-it-works)
- [Installation](#-installation)
- [Usage](#-usage)
- [Debug Features](#-debug-features)
- [Performance Optimizations](#-performance-optimizations)
- [Testing](#-testing)
- [Technical Details](#-technical-details)

---

## ✨ Features

### Core Functions
- ✅ `malloc(size_t size)` - Allocate memory
- ✅ `free(void *ptr)` - Free allocated memory
- ✅ `realloc(void *ptr, size_t size)` - Resize allocated memory
- ✅ `calloc(size_t nmemb, size_t size)` - Allocate and zero-initialize memory
- ✅ `show_alloc_mem()` - Display memory allocation map
- ✅ `show_alloc_mem_ex()` - Extended memory map with hexdump

### Advanced Features
- 🔒 **Thread-Safe** - Protected by mutexes for concurrent access
- 🚀 **Performance Optimized** - Free lists, cached zone ends, and block coalescing
- 🐛 **Debug Tools** - Comprehensive debugging via environment variables
- 🎨 **Color Output** - ANSI colored output for better visibility
- 📊 **Statistics** - Track allocations, frees, and memory leaks
- 🔍 **Hexdump** - Visualize memory contents in hex and ASCII

---

## 🏗️ How It Works

### Memory Zones

The allocator organizes memory into three size categories:

| Category | Size Range | Implementation |
|----------|-----------|----------------|
| **TINY** | 0 - 128 bytes | Pre-allocated zones with ~100 blocks |
| **SMALL** | 129 - 1024 bytes | Pre-allocated zones with ~100 blocks |
| **LARGE** | 1025+ bytes | Individual `mmap` allocations |

### Memory Layout

```
┌─────────────────────────────────────────┐
│  TINY Zone (multiple blocks)            │
│  ┌──────────┬──────────┬──────────┐    │
│  │ Block 1  │ Block 2  │ Block 3  │ ...│
│  └──────────┴──────────┴──────────┘    │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│  SMALL Zone (multiple blocks)           │
│  ┌──────────┬──────────┬──────────┐    │
│  │ Block 1  │ Block 2  │ Block 3  │ ...│
│  └──────────┴──────────┴──────────┘    │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│  LARGE Allocation (single block)        │
│  ┌──────────────────────────────────┐   │
│  │         Large Block              │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

Each block contains:
- **Metadata**: Size, free status, next/prev pointers
- **User Data**: The actual allocated memory
- **Free List**: Separate linked list for quick free block lookup

---

## 🔧 Installation

### Build the Library

```bash
# Clone the repository
git clone https://github.com/goncalo1021pt/malloc.git
cd malloc

# Compile the library
make

# This creates:
# - libft_malloc_x86_64.so
# - libft_malloc.so (symlink)
```

### Clean

```bash
make clean    # Remove object files
make fclean   # Remove library
make re       # Rebuild everything
```

---

## 🚀 Usage

### Method 1: Using LD_PRELOAD (Recommended for Testing)

Replace the system malloc for any program:

```bash
# Run with your malloc
LD_PRELOAD=./libft_malloc_x86_64.so /bin/ls

# Test with your own programs
LD_PRELOAD=./libft_malloc_x86_64.so ./your_program

# Even works with bash!
LD_PRELOAD=./libft_malloc_x86_64.so bash
```

### Method 2: Link Directly

```bash
# Compile your program with the library
cc your_program.c -L. -lft_malloc -Wl,-rpath,. -o your_program

# Run normally
./your_program
```

### Basic Example

```c
#include <stdlib.h>
#include <string.h>

int main() {
    // Allocate memory
    char *str = malloc(100);
    strcpy(str, "Hello, World!");
    
    // Resize
    str = realloc(str, 200);
    
    // Free
    free(str);
    
    // Allocate and zero-initialize
    int *array = calloc(10, sizeof(int));
    free(array);
    
    return 0;
}
```

---

## 🐛 Debug Features

### Environment Variables

Control debugging behavior with environment variables:

```bash
# Enable verbose logging
FT_MALLOC_DEBUG=1

# Show statistics on program exit
FT_MALLOC_STATS=1

# Detect double-free errors
FT_MALLOC_CHECK=1

# Fill memory with pattern (0-255)
FT_MALLOC_PERTURB=170

# Combine multiple flags
LD_PRELOAD=./libft_malloc_x86_64.so \
  FT_MALLOC_DEBUG=1 \
  FT_MALLOC_STATS=1 \
  FT_MALLOC_CHECK=1 \
  FT_MALLOC_PERTURB=170 \
  ./your_program
```

### Debug Output Examples

#### FT_MALLOC_DEBUG=1
```
[MALLOC] size=32 -> 0x7f1234560060
[MALLOC] size=64 -> 0x7f1234560110
[FREE] 0x7f1234560060
[REALLOC] 0x7f1234560110 -> 0x7f1234560180 size=128
```

#### FT_MALLOC_STATS=1
```
=== MALLOC STATISTICS ===
Total malloc calls: 57
Total free calls: 57
Total realloc calls: 0
No memory leaks detected
```

#### FT_MALLOC_CHECK=1
```
[ERROR] Double-free detected at 0x7f1234560060
```

#### FT_MALLOC_PERTURB=170
Fills allocated memory with `0xAA` and freed memory with `0x55`:
```
After malloc:  aa aa aa aa aa aa aa aa
After free:    55 55 55 55 55 55 55 55
```

### Extended Memory View

```c
void show_alloc_mem_ex(void);
```

Shows detailed memory map with hexdump:

```
=== EXTENDED MEMORY ALLOCATION MAP ===

TINY : 0x7f1234560000
  [ALLOCATED] 0x7f1234560060 - 0x7f1234560080 : 32 bytes
    0x7f1234560060: 48 65 6c 6c 6f 2c 20 57  6f 72 6c 64 21 00 00 00  |Hello, World!...|
    0x7f1234560070: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
  
  [FREE] 0x7f12345600b0 - 0x7f12345600f0 : 64 bytes
    0x7f12345600b0: 55 55 55 55 55 55 55 55  55 55 55 55 55 55 55 55  |UUUUUUUUUUUUUUUU|

=== STATISTICS ===
Total zones: 2 (1 TINY, 1 SMALL, 0 LARGE)
Allocated: 1120 bytes in 4 blocks
Free: 64 bytes in 1 blocks
Fragmentation: 5%
```

---

## ⚡ Performance Optimizations

### 1. **Cached Zone End Pointer**
- Eliminates O(n) traversal when allocating new blocks
- Stores the end of used memory in each zone
- Updated incrementally during allocation

### 2. **Free List**
- Separate linked list for free blocks per zone
- O(free blocks) search instead of O(all blocks)
- Fast reuse of freed memory

### 3. **Block Coalescing**
- Automatically merges adjacent free blocks
- Reduces memory fragmentation
- Maintains larger contiguous free spaces

### 4. **Smart Zone Management**
- Zones are only unmapped when completely empty
- Keeps at least one zone per category to avoid thrashing
- Efficient reuse of memory

### Performance Results

Testing with real programs (e.g., `bash`, `ls`):
```bash
# Your malloc is fast enough to run bash!
LD_PRELOAD=./libft_malloc_x86_64.so bash

# Test with ls
LD_PRELOAD=./libft_malloc_x86_64.so FT_MALLOC_STATS=1 ls
# Result: 57 mallocs, 57 frees, 0 leaks ✓
```

---

## 🧪 Testing

### Test Suite

The `tests/` directory contains comprehensive tests:

```bash
cd tests
make              # Build all tests
make run_all      # Run all tests

# Individual tests
make run_perturb        # Memory pattern testing
make run_debug          # Logging and statistics
make run_advanced       # Double-free and hexdump
make run_comprehensive  # Full stress test
```

### Test Programs

1. **test_perturb.c** - Tests memory fill patterns
2. **test_debug.c** - Tests logging and leak detection
3. **test_advanced.c** - Tests hexdump and double-free detection
4. **test_comprehensive.c** - Stress test with all features

### Example Test Run

```bash
$ make run_perturb
=== FT_MALLOC_PERTURB Test ===

After malloc(16):
  Content: aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa
  Expected: AA AA AA AA... (if PERTURB=170)

✓ SUCCESS: All bytes are 0xAA (perturb is working!)
```

---

## 📚 Technical Details

### Data Structures

```c
typedef struct s_block {
    size_t size;              // Block size
    bool free;                // Free/allocated status
    struct s_block *next;     // Next block in zone
    struct s_block *prev;     // Previous block (for coalescing)
    struct s_block *free_next; // Next in free list
    char data[1];             // User data (flexible array)
} t_block;

typedef struct s_zone {
    void *start;              // Zone start address
    size_t zone_size;         // Total zone size
    int blocks_allocated;     // Number of allocated blocks
    int blocks_free;          // Number of free blocks
    struct s_zone *next;      // Next zone
    t_block *blocks;          // First block in zone
    t_block *free_list;       // Head of free blocks list
    void *zone_end;           // Cached end pointer
} t_zone;
```

### Thread Safety

All operations are protected by a global mutex:
```c
pthread_mutex_t mutex;
pthread_mutex_lock(&g_malloc_metadata.mutex);
// ... critical section ...
pthread_mutex_unlock(&g_malloc_metadata.mutex);
```

### Memory Alignment

- All allocations are 16-byte aligned for optimal performance
- Uses `ALIGN(size)` macro: `(((size) + 15) & ~15)`

### System Calls

- **mmap** - Allocate memory from OS
- **munmap** - Return memory to OS
- **getpagesize** - Get system page size for alignment

---

## 📖 Resources

- **glibc malloc**: Inspired by GNU C Library's malloc implementation
- **The C Programming Language** by Kernighan & Ritchie
- **Understanding the Linux Kernel** by Bovet & Cesati

---

## 👤 Author

**goncalo1021pt**

- GitHub: [@goncalo1021pt](https://github.com/goncalo1021pt)
- 42 School Project

---

## 📝 License

This project is part of the 42 School curriculum.

---

## 🎯 Project Goals Achieved

- ✅ Implement malloc/free/realloc/calloc
- ✅ Thread-safe implementation
- ✅ Memory defragmentation (block coalescing)
- ✅ Debug environment variables (MALLOC_PERTURB_, MALLOC_CHECK_, etc.)
- ✅ Extended memory visualization (show_alloc_mem_ex)
- ✅ Performance optimizations (free lists, cached zone ends)
- ✅ Comprehensive test suite
- ✅ Real-world testing (bash, ls, etc.)

<!-- **Grade: 125/100** ⭐ -->

---

<div align="center">

**Made with ❤️ at 42 School**

</div>