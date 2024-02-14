#include "event.h"
#include "kernelEv.h"
#include "system.h"

Event::Event(IVTNo ivtNo){
	lock();
	myImpl = new KernelEv((PCB*)PCB::running, ivtNo);
	unlock();
}

Event::~Event(){
	lock();
	delete myImpl;
	unlock();
}

void Event::wait(){
	lock();
	myImpl->wait();
	unlock();
}

void Event::signal(){
	lock();
	myImpl->signal();
	unlock();
}
