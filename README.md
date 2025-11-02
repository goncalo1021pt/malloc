# malloc

This project implements a custom memory allocator in C, providing replacements for the standard `malloc`, `free`, `realloc`, and `calloc` functions. The allocator is designed to manage memory efficiently and safely, with features such as thread safety and memory leak detection.

## Size thresholds
TINY: 0 to 128 bytes  
SMALL: 129 to 1024 bytes
LARGE: 1025 bytes and above