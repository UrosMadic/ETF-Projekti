//
// Created by os on 7/25/22.
//

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"
struct MemHeader{
    MemHeader *sled;
    size_t size;
    char* addr;
};




class MemoryAllocator{
private:
    static MemHeader* prvi;
    static int i;

    MemoryAllocator(){}
public:
    static void* mem_alloc();
    static int mem_free();
};


#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_HPP
