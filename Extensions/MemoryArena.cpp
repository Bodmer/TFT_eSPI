// MemoryArena.cpp
#include "MemoryArena.hpp"
#include <algorithm>
#include <cstring>

MemoryArena::MemoryArena(size_t initialSize) : memory(initialSize), currentOffset(0) {}

MemoryArena::~MemoryArena() = default;

void* MemoryArena::allocate(size_t size) {
    if (currentOffset + size > memory.size()) {
        size_t newSize = std::max(memory.size() * 2, currentOffset + size);
        memory.resize(newSize);
    }

    void* ptr = &memory[currentOffset];
    currentOffset += size;
    return ptr;
}

void MemoryArena::reset() {
    currentOffset = 0;
}

void MemoryArena::resetToZero() {
    std::fill(memory.begin(), memory.end(), 0);
    currentOffset = 0;
}
