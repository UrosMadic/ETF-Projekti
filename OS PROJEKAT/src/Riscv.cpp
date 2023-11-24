//
// Created by os on 7/29/22.
//
#include "../h/Riscv.hpp"
#include "../lib/console.h"
#include "../h/PCB.hpp"


void Riscv::popSppSpie(){
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap(){

    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL){
        uint64 a00;
        __asm__ volatile("mv %0, a0":"=r"(a00));
        if(a00==0x01){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            MemoryAllocator::mem_alloc();
            w_sstatus(sstatus);
            w_sepc(sepc);

        }
        else if(a00==0x02){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            MemoryAllocator::mem_free();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        else if(a00==0x11){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            PCB::createThread();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        else if(a00==0x12){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            PCB::exit();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        else if(a00==0x13){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            //PCB::timeSliceCounter = 0;
            PCB::yield();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        else if(a00==0x21){

        }
        else if(a00==0x22){

        }
        else if(a00==0x23){

        }
        else if(a00==0x24){

        }
        else if(a00==0x31){

        }
        else if(a00==0x41){
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            __getc();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        else if(a00==0x42){
            char z;
            __asm__ volatile("mv %0, a1":"=r"(z));
            uint64 volatile sepc = r_sepc() + 4;
            uint64 volatile sstatus = r_sstatus();
            __putc(z);
            w_sstatus(sstatus);
            w_sepc(sepc);
        }

    }
    else if (scause == 0x8000000000000001UL){

        mc_sip(SIP_SSIP);
//        PCB::timeSliceCounter++;
//        if (PCB::timeSliceCounter >= PCB::running->getTimeSlice())
//        {
//            uint64 volatile sepc = r_sepc();
//            uint64 volatile sstatus = r_sstatus();
//            PCB::timeSliceCounter = 0;
//            PCB::yield();
//            w_sstatus(sstatus);
//            w_sepc(sepc);
//        }

    }
    else if (scause == 0x8000000000000009UL){

        console_handler();
    }
    else{
    }
}

