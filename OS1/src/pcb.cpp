#include <dos.h>

#include "SCHEDULE.H"
#include "pcb.h"
#include "system.h"
#include "idle.h"

ID PCB::pcb_id = 0;

Thread* PCB::initial = 0;
volatile PCB* PCB::running = 0;
Thread* PCB::idle = 0;
PCB* PCB::pcb_idle = 0;

void PCB::sys_inic(){
	lock();

	inic(); //timer

	PCB::initial = new Thread();
	PCB::initial->myPCB->ready = 1;
	PCB::running = PCB::initial->myPCB;

	PCB::idle = new Idle();
	PCB::pcb_idle = PCB::idle->myPCB;
	PCB::pcb_idle->ready = 1;
	PCB::pcb_idle->init_context();

	unlock();
}

void PCB::sys_restore(){
	lock();

	restore(); //timer

	delete PCB::initial;
	delete PCB::idle;
	delete threads;

	unlock();
}

PCB::PCB(Thread* myThread, StackSize stackSize, Time timeSlice){
	lock();

	//velicina steka
	if(stackSize > 65535 || stackSize <= 0) stackSize = defaultStackSize;
	stackSize /= sizeof(unsigned);
	this->stackSize = stackSize;

	//inicijalizacija polja pcb
	id = pcb_id++;
	this->timeSlice = timeSlice;
	this->myThread = myThread;
	waiting = 0; //postavlja semafor
	finished = blocked = ready = 0;

	waiting_queue = new Queue();

	wait_ret = 1; //nije se ni blokirao na semaforu

	unlock();
}

PCB::PCB(){
	lock();
	PCB(0, defaultStackSize, defaultTimeSlice);
	unlock();
}

void PCB::init_context(){
	lock();
	stack = new unsigned[stackSize];

	//inicijalizacija steka
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 1] = FP_SEG(myThread);
	stack[stackSize - 2] = FP_OFF(myThread);
	//
	stack[stackSize - 5] = 0x200;
	stack[stackSize - 6] = FP_SEG(PCB::wrapper);
	stack[stackSize - 7] = FP_OFF(PCB::wrapper);

	ss = FP_SEG(stack + stackSize - 16);
	sp = FP_OFF(stack + stackSize - 16);
	bp = FP_OFF(stack + stackSize - 16);
#endif
	unlock();
}

PCB::~PCB(){
	lock();
	delete waiting_queue;
	delete stack;
	unlock();
}

//f-ja koja okruzuje run()
void PCB::wrapper(){
	PCB::running->myThread->run();

	lock();
	while(PCB::running->waiting_queue->n > 0){
		PCB* pcb = PCB::running->waiting_queue->get();
		pcb->blocked = 0;
		if(!pcb->finished) {
			pcb->ready = 1;
			Scheduler::put(pcb);
		}
	}

	PCB::running->finished = 1;
	PCB::running->ready = 0;
	dispatch();
	unlock();
}
