#include "SCHEDULE.H"
#include "kernSem.h"
#include "semaphor.h"
#include "pcb.h"
#include "system.h"
#include "thread.h"
#include "queue.h"

int KernelSem::sem_id = 0;

KernelSem::KernelSem(int init){ //ne mora lock() i unlock() jer ga stiti klasa Semaphore
	value = init;
	id = sem_id++;
	blocked_queue = new Queue();
}

KernelSem::~KernelSem(){
	if(blocked_queue != 0){
		while(blocked_queue->n > 0){
			deblock();
		}
		delete blocked_queue;
	}
	semaphores[id] = 0;
}

int KernelSem::wait(Time maxTimeToWait){
	lock();

	PCB::running->waiting = maxTimeToWait;

	if (--value < 0)
		block();
	else PCB::running->wait_ret = 1;

	unlock();
	return PCB::running->wait_ret;
}

void KernelSem::signal(){
	if (value++ < 0)
		deblock();
}

int KernelSem::val() const{
	return value;
}

void KernelSem::block(){
	PCB::running->blocked = 1;
	PCB::running->ready = 0;
	blocked_queue->put((PCB*)PCB::running);
	dispatch();
}

void  KernelSem::deblock(){
	PCB* pcb = blocked_queue->get();
	//promeni status blocked and ready
	if(pcb){
		pcb->blocked = 0;
		pcb->ready = 1;
		pcb->wait_ret = 1;
		Scheduler::put(pcb);
	}
}

