
#include "../h/Scheduler.hpp"

List<PCB> Scheduler::readyThreadQueue;

PCB *Scheduler::get(){
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(PCB *pcb){
    readyThreadQueue.addLast(pcb);
}