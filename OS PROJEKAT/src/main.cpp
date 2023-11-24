#include "../lib/hw.h"
#include "../h/printing.hpp"
#include "../h/Riscv.hpp"
#include "../h/PCB.hpp"
#include "../h/syscall_cpp.hpp"
#include "../lib/console.h"



void userMain();

void main(){
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    __asm__ volatile("mv a2, %0" : :"r"(nullptr));
    thread_t mainThread=(thread_t)PCB::createThread2();
    PCB::running=mainThread;

    void* stack_space=mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a7, %0": :"r"(stack_space));
    __asm__ volatile("mv a2, %0" : :"r"(userMain));
    thread_t userMainThread=(thread_t)PCB::createThread2();
    while(!(userMainThread->isFinished())){
        thread_dispatch();
    }


}


