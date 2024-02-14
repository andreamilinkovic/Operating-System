#include "SCHEDULE.H"
#include "thread.h"
#include "pcb.h"
#include "system.h"

 Thread::Thread(StackSize stackSize, Time timeSlice){
	 lock();
	 myPCB = new PCB(this, stackSize, timeSlice);
	 threads->put(myPCB);
	 unlock();
 }

 void Thread::start(){
 	lock();
 	if(!myPCB->blocked && !myPCB->finished && !myPCB->ready){
 		myPCB->ready = 1;
 		myPCB->init_context();
 		Scheduler::put(myPCB);
 	}
 	unlock();
 }

void Thread::waitToComplete(){ //TODO
	lock();
	if(!myPCB->finished && !(myPCB == PCB::running) && !(this == PCB::initial) && !(this == PCB::idle)){
		PCB::running->blocked = 1;
		PCB::running->ready = 0;
		myPCB->waiting_queue->put((PCB*)PCB::running);
		dispatch();
	}
	unlock();
}

Thread::~Thread(){
	lock();
	waitToComplete();
	//remove from list of threads
	threads->remove(myPCB->id);
	delete myPCB;
	unlock();
}

ID Thread::getId(){
	return myPCB->id;
}

ID Thread::getRunningId(){
	return PCB::running->id;
}

Thread* Thread::getThreadById(ID id){
	return threads->find(id);
}

void dispatch (){
	lock();
	sys_dispatch();
	unlock();
}
