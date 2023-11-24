//
// Created by os on 8/2/22.
//

#ifndef PROJECT_BASE_V1_1_PCB_HPP
#define PROJECT_BASE_V1_1_PCB_HPP
#include "../lib/hw.h"
#include "syscall_c.hpp"
#include "Scheduler.hpp"


class PCB {
public:
    ~PCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    //uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    using Body2 = void (*)();

    static PCB *createThread();
    static PCB *createThread2();
    static void yield();

    static PCB *running;
    static void *operator new(size_t size);

    static void *operator new[](size_t size);

    static void operator delete(void *p) noexcept;
    static void operator delete[](void *p) noexcept;

private:
    PCB(Body body,void* arg1,uint64* stack_size):body(body),arg1(arg1),stack(body != nullptr ? stack_size: nullptr),
    context({(uint64)&threadWrapper,stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0}),
    finished(false){
        if (body != nullptr) { Scheduler::put(this);}
    }

    PCB(Body2 body2,uint64* stack_size):body2(body2),stack(body2 != nullptr ? stack_size: nullptr),
    context({(uint64)&threadWrapper2,stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0}),finished(false){
        if (body2 != nullptr) { Scheduler::put(this);}
    }


    struct Context{
        uint64 ra;
        uint64 sp;
    };

    Body body;
    Body2 body2;
    void* arg1;
    uint64 *stack;
    Context context;
    //uint64 timeSlice;
    bool finished;

    friend class Riscv;

    static void threadWrapper();
    static void threadWrapper2();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();
    static int exit();


    //static uint64 timeSliceCounter;


};



#endif //PROJECT_BASE_V1_1_PCB_HPP
