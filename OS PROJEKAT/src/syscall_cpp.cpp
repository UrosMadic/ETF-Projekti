//
// Created by os on 8/9/22.
//

#include "../h/syscall_cpp.hpp"
#include "../h/Scheduler.hpp"


void *operator new(size_t size){
    return mem_alloc(size);
}

void operator delete(void *p) {
    mem_free(p);
}


Thread::Thread(void (*body)(void *), void *arg) {
    thread_create(&myHandle,body,arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::start() {
    Scheduler::put(myHandle);
    return 0;
}

Thread::Thread() {
    thread_create(&myHandle,threadWrapper2, this);
}

Thread::~Thread() {

}

void Thread::threadWrapper2(void * arg) {
    ((Thread*)arg)->run();

}


char Console::getc() {
    return ::getc();
}

void Console::putc(char z) {
    ::putc(z);
}

