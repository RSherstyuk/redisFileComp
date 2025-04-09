Simple implementation of a Redis queue with producer and consumer

# File Compressor

A multithreaded file compressor using hiredis and zlib.

## Requirements
- C++17
- CMake 3.10+
- Zlib
- Hiredis

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

Usage:

## Consumer
./consumer <outpur_dir>

## Producer
./producer <input_dir>
