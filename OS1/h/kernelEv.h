#ifndef _kernelEv_h_
#define _kernelEv_h_

#include <dos.h>

#include "pcb.h"
#include "event.h"

typedef void interrupt(*pInterrupt)(...);

#define PREPAREENTRY(numEntry, callOld)\
	void interrupt inter##numEntry(...); \
	IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
	void interrupt inter##numEntry(...) {\
		newEntry##numEntry.signal();\
		if (callOld == 1)\
			newEntry##numEntry.oldISR();\
	}

class KernelEv{
public:
	KernelEv(PCB* owner, IVTNo ivtNo);

	~KernelEv();

	void wait();

	void signal();

	PCB *owner, *blocked;

	int val;

	int numEntry;
};


class IVTEntry{
public:
	IVTEntry(int numEntry, pInterrupt inter);

	pInterrupt oldISR; //stara rutina

	void signal();

	KernelEv* kernEv;
};

#endif
