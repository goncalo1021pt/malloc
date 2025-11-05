# Malloc Test Suite

This directory contains comprehensive tests for the ft_malloc implementation.

## Test Programs

### 1. `test_perturb.c`
Tests the `FT_MALLOC_PERTURB` feature which fills memory with patterns.

**Run:**
```bash
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_PERTURB=170 ./test_perturb
```

**What it tests:**
- Uninitialized memory detection (fills with 0xAA)
- Use-after-free detection (fills freed memory with 0x55)
- Pattern verification

---

### 2. `test_debug.c`
Tests `FT_MALLOC_DEBUG` and `FT_MALLOC_STATS` features.

**Run with debug logging:**
```bash
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_DEBUG=1 ./test_debug
```

**Run with statistics:**
```bash
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_STATS=1 ./test_debug
```

**What it tests:**
- Verbose logging of malloc/free/realloc
- Statistics tracking (total calls, memory leaks)
- Free block reuse
- Memory leak detection

---

### 3. `test_advanced.c`
Tests `FT_MALLOC_CHECK` and `show_alloc_mem_ex()` features.

**Run:**
```bash
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_CHECK=1 ./test_advanced
```

**What it tests:**
- Double-free detection
- Extended memory map with hexdump
- Fragmentation visualization
- Free block display

---

### 4. `test_comprehensive.c`
Stress test with all features combined.

**Run:**
```bash
LD_PRELOAD=../libft_malloc_x86_64.so \
  FT_MALLOC_DEBUG=1 \
  FT_MALLOC_STATS=1 \
  FT_MALLOC_CHECK=1 \
  FT_MALLOC_PERTURB=170 \
  ./test_comprehensive
```

**What it tests:**
- TINY allocations (≤128 bytes)
- SMALL allocations (≤1024 bytes)
- LARGE allocations (>1024 bytes)
- Realloc patterns
- Fragmentation under stress

---

## Quick Start

### Build all tests:
```bash
make
```

### Run individual tests:
```bash
make run_perturb
make run_debug
make run_advanced
make run_comprehensive
```

### Run all tests:
```bash
make run_all
```

### Clean up:
```bash
make clean
```

---

## Environment Variables

| Variable | Values | Description |
|----------|--------|-------------|
| `FT_MALLOC_DEBUG` | 0/1 | Enable verbose logging to stderr |
| `FT_MALLOC_STATS` | 0/1 | Show statistics on program exit |
| `FT_MALLOC_CHECK` | 0/1 | Enable double-free detection |
| `FT_MALLOC_PERTURB` | 0-255 | Fill pattern (alloc=value, free=~value) |

---

## Example Outputs

### Debug Logging:
```
[MALLOC] size=32 -> 0x7f1234560060
[MALLOC] size=64 -> 0x7f1234560110
[FREE] 0x7f1234560060
[REALLOC] 0x7f1234560110 -> 0x7f1234560180 size=128
```

### Statistics:
```
=== MALLOC STATISTICS ===
Total malloc calls: 42
Total free calls: 40
Total realloc calls: 2
Potential memory leaks: 2 allocations not freed
```

### Double-Free Detection:
```
[ERROR] Double-free detected at 0x7f1234560060
```

### Memory Perturb:
```
After malloc(16):
  Content: aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa
After free:
  Content: 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55 55
```

---

## Testing with Real Programs

Test with any program using `LD_PRELOAD`:

```bash
# Test with ls
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_STATS=1 /bin/ls

# Test with your own programs
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_DEBUG=1 ./your_program

# Test with bash
LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_STATS=1 bash -c "echo hello"
```
