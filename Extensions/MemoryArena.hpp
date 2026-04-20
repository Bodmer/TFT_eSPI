// MemoryArena.hpp
#pragma once
#include <vector>

class MemoryArena {
public:
    explicit MemoryArena(size_t initialSize);
    ~MemoryArena();

    void* allocate(size_t size);
    void reset();
    void resetToZero();

private:
    std::vector<char> memory;
    size_t currentOffset;
};
