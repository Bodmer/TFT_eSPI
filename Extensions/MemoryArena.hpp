// MemoryArena.h
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

class MemoryArena {
public:
    MemoryArena(size_t initialSize = 1024 * 1024);  // Default 1MB
    ~MemoryArena();

    void* allocate(size_t size);
    void reset();

private:
    std::vector<uint8_t> memory;
    size_t currentOffset;
};
