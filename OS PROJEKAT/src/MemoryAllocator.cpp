//
// Created by os on 7/25/22.
//

#include "../h/MemoryAllocator.hpp"

int MemoryAllocator::i=0;
MemHeader *MemoryAllocator::prvi;
//AddrHeader *MemoryAllocator::prvi1;

void *MemoryAllocator::mem_alloc() {
    uint64 a11;
    __asm__ volatile("mv %0, a1":"=r"(a11));
    size_t ukVel=a11*MEM_BLOCK_SIZE;
    if(i==0){
        prvi=(MemHeader*)(size_t)HEAP_START_ADDR;
        prvi->size=(size_t)HEAP_END_ADDR-(size_t)HEAP_START_ADDR-(size_t)sizeof(MemHeader);
        prvi->addr=(char*)HEAP_START_ADDR+(size_t)sizeof(MemHeader);
        prvi->sled=nullptr;
        i=1;
    }
    MemHeader* tek=prvi,*preth=nullptr;

    for(;tek!= nullptr;preth=tek,tek=tek->sled){
        if(tek->size>=ukVel)break;
    }
    if(tek==nullptr){
        return nullptr;
    }
    size_t slobMem=tek->size-ukVel;
    if(slobMem> sizeof(MemHeader)) {
        tek->size = ukVel;
        size_t offset = sizeof(MemHeader) + ukVel;
        MemHeader *novi = (MemHeader *) ((char *) tek + offset);
        if (preth)preth->sled = novi;
        else prvi = novi;
        novi->sled = tek->sled;
        novi->size = slobMem - sizeof(MemHeader);
        novi->addr=(char*)tek+offset+sizeof(MemHeader);
    } else{
        if(preth)preth->sled=tek->sled;
        else prvi=tek->sled;
    }

    tek->sled=nullptr;
    return (char*)tek+sizeof(MemHeader);
}

int MemoryAllocator::mem_free() {
    void* a11;
    __asm__ volatile("mv %0, a1":"=r"(a11));
    if(a11== nullptr){
        return -1;
    }

    char* adr=(char*)a11-sizeof(MemHeader);

    MemHeader* tek;
    if(!prvi || adr<(char*)prvi){
        tek=nullptr;
    }else{
        for(tek=prvi;tek->sled!=0 && adr>(char*)(tek->sled);tek=tek->sled);
    }

    MemHeader* novi=(MemHeader*)adr;
    if(novi->addr!=a11){
        return -1;
    }
    if(tek)novi->sled=tek->sled;
    else novi->sled=prvi;
    if(tek)tek->sled=novi;
    else prvi=novi;


    if(novi->sled && (char*)novi+sizeof(MemHeader)+novi->size==(char*)(novi->sled)){
        novi->size+=novi->sled->size;
        novi->sled=novi->sled->sled;
    }

    if(!tek)return 0;
    if(tek->sled && (char*)tek+sizeof(MemHeader)+tek->size==(char*)(tek->sled)){
        tek->size+=tek->sled->size;
        tek->sled=tek->sled->sled;
    }
    return 0;
}
