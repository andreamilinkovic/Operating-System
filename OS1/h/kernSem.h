#ifndef _kernSem_h_
#define _kernSem_h_

#include "queue.h"

class KernelSem{
public:
	KernelSem(int init);

	virtual ~KernelSem();

	virtual int wait(Time maxTimeToWait);

	virtual void signal();

	int val() const;

	static int sem_id;

	int id;

	Queue* blocked_queue; //red blokiranih niti na semaforu

	int value;

	void block();

	void deblock();
};

#endif
