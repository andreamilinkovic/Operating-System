#include <dos.h>

#include "SCHEDULE.H"
#include "kernelEv.h"
#include "system.h"

KernelEv::KernelEv(PCB* owner, IVTNo ivtNo){
	this->owner = owner;
	blocked = 0;
	val = 0;
	numEntry = ivtNo;
	if(entries[ivtNo] && entries[ivtNo]->kernEv == 0)
		entries[ivtNo]->kernEv = this;
}

KernelEv::~KernelEv(){
#ifndef BCC_BLOCK_IGNORE
	setvect(numEntry, entries[numEntry]->oldISR);
#endif
	entries[numEntry] = 0;
}

void KernelEv::wait(){
	if(owner == (PCB*)PCB::running){
		if(val == 0){
			owner->blocked = 1;
			owner->ready = 0;
			blocked = owner;
			dispatch();
		}
		else
			if(val == 1) val = 0;
	}
}

void KernelEv::signal(){
	if(blocked == 0)
		val = 1;
	else{
		owner->blocked = 0;
		owner->ready = 1;
		Scheduler::put(owner);
		blocked = 0;
	}
}


IVTEntry::IVTEntry(int numEntry, pInterrupt inter){
	lock();
	kernEv = 0;
	entries[numEntry] = this;
#ifndef BCC_BLOCK_IGNORE
	oldISR = getvect(numEntry);
	setvect(numEntry, inter);
#endif
	unlock();
}

void IVTEntry::signal(){
	lock();
	kernEv->signal();
	unlock();
}
