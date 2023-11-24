//
// Created by os on 8/5/22.
//

#include "../h/syscall_c.hpp"


void* mem_alloc (size_t size){
    size_t brojBlok;

    brojBlok=size/((size_t)MEM_BLOCK_SIZE);
    if(size%((size_t)MEM_BLOCK_SIZE))brojBlok++;
    size_t a01;
    size_t a10;
    __asm__ volatile("mv %0,a0" : "=r"(a10));
    __asm__ volatile("mv a1, %0": :"r"(brojBlok));
    __asm__ volatile("mv a0,%0" : : "r"(0x01));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0":"=r"(a01));
    return (char*)a01;
//    return MemoryAllocator::mem_alloc(size);
}

int mem_free (void* addr){
    void* a10;
    int a;
    __asm__ volatile("mv %0,a0" : "=r"(a10));
    __asm__ volatile("mv a1, %0": :"r"(a10));
    __asm__ volatile("mv a0,%0" : : "r"(0x02));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0":"=r"(a));
    return a;
//    return MemoryAllocator::mem_free(addr);
}

int thread_create (thread_t* handle,void(*start_routine)(void*),void* arg){

    void* stack_space= mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a7, %0": :"r"(stack_space));
    __asm__ volatile("mv a6, %0": :"r"(arg));
    __asm__ volatile("mv a2, %0": :"r"(start_routine));
    __asm__ volatile("mv a1, %0": :"r"(handle));
    __asm__ volatile("mv a0,%0" : : "r"(0x11));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0":"=r"(*handle));
    if(*handle) {
        return 0;
    }
    else return -1;

}



int thread_exit (){
    int a;
    __asm__ volatile("mv a0,%0" : : "r"(0x12));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0":"=r"(a));
    return a;
}

void thread_dispatch (){
    __asm__ volatile("mv a0,%0" : : "r"(0x13));
    __asm__ volatile("ecall");

}

char getc (){
    char a;
    __asm__ volatile("mv a0,%0" : : "r"(0x41));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %0,a0":"=r"(a));
    return a;

}

void putc(char z){
    __asm__ volatile("mv a1, %0": :"r"(z));
    __asm__ volatile("mv a0,%0" : : "r"(0x42));
    __asm__ volatile("ecall");
}


