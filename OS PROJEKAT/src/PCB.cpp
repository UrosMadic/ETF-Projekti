//
// Created by os on 8/2/22.
//

#include "../h/PCB.hpp"
#include "../h/Riscv.hpp"

PCB *PCB::running = nullptr;

//uint64 PCB::timeSliceCounter = 0;

PCB *PCB::createThread(){
    void* arg;
    Body body;
    thread_t* handle;
    uint64* stack_space;
    __asm__ volatile("mv %0, a2":"=r"(body));

    __asm__ volatile("mv %0, a6":"=r"(arg));

    __asm__ volatile("mv %0, a1":"=r"(handle));

    __asm__ volatile("mv %0, a7":"=r"(stack_space));

    return new PCB(body, arg, stack_space);

}
PCB *PCB::createThread2(){
    Body2 body2;
    uint64* stack_space;
    __asm__ volatile("mv %0, a2":"=r"(body2));
    if(body2==nullptr){
        stack_space= nullptr;
    }else{
        __asm__ volatile("mv %0, a7":"=r"(stack_space));
    }
    return new PCB(body2,stack_space);

}

void PCB::dispatch(){
    __asm__ volatile("mv a0,%0" : : "r"(0x13));
    __asm__ volatile ("ecall");

}

void PCB::yield(){
    PCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    PCB::contextSwitch(&old->context, &running->context);

}

void PCB::threadWrapper(){
    Riscv::popSppSpie();
    running->body(running->arg1);
    running->setFinished(true);
    PCB::dispatch();
}
void PCB::threadWrapper2(){
    Riscv::popSppSpie();
    running->body2();
    running->setFinished(true);
    PCB::dispatch();
}


int PCB::exit() {
    running->finished=true;
    PCB *old = running;
    running = Scheduler::get();
    PCB::contextSwitch(&old->context, &running->context);
    return 0;
}


void *PCB::operator new(size_t size){
    size_t brojBlok;

    brojBlok=size/((size_t)MEM_BLOCK_SIZE);
    if(size%((size_t)MEM_BLOCK_SIZE))brojBlok++;
    __asm__ volatile("mv a1,%0" : :"r"(brojBlok));
    return MemoryAllocator::mem_alloc();
}

void *PCB::operator new[](size_t size){
    size_t brojBlok;

    brojBlok=size/((size_t)MEM_BLOCK_SIZE);
    if(size%((size_t)MEM_BLOCK_SIZE))brojBlok++;
    __asm__ volatile("mv a1,%0" : :"r"(brojBlok));
    return MemoryAllocator::mem_alloc();
}


void PCB::operator delete(void *p) noexcept{
    __asm__ volatile("mv a1,%0" : :"r"(p));
    MemoryAllocator::mem_free();
}

void PCB::operator delete[](void *p) noexcept{
    __asm__ volatile("mv a1,%0" : :"r"(p));
    MemoryAllocator::mem_free();
}


