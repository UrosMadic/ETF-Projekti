//
// Created by os on 8/2/22.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_HPP
#define PROJECT_BASE_V1_1_SCHEDULER_HPP
#include "List.hpp"


class PCB;

class Scheduler
{
private:
    static List<PCB> readyThreadQueue;

public:
    static PCB *get();

    static void put(PCB *pcb);

};


#endif //PROJECT_BASE_V1_1_SCHEDULER_HPP
