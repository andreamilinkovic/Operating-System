#include "semaphor.h"
#include "system.h"
#include "kernSem.h"

Semaphore::Semaphore(int init){
	lock();
	myImpl = new KernelSem(init);
	semaphores[myImpl->id] = myImpl;
	unlock();
}

Semaphore::~Semaphore(){
	lock();
	delete myImpl;
	unlock();
}


int Semaphore::wait (Time maxTimeToWait){
	return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal(){
	lock();
	myImpl->signal();
	unlock();
}

int Semaphore::val() const{
	return myImpl->val();
}
