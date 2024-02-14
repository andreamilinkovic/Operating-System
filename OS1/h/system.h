#ifndef _system_h_
#define _system_h_

#include "queue.h"
#include "kernSem.h"
#include "kernelEv.h"

// Zabranjuje prekide
#define lock() asm{\
	pushf;\
	cli;\
}

// Dozvoljava prekide
#define unlock() asm popf

extern Queue* threads;
extern KernelSem* semaphores[256];
extern IVTEntry* entries[256];

extern void tick();

extern volatile unsigned int lockFlag;

void interrupt timer();

// postavlja novu prekidnu rutinu timera
void inic();

// vraca staru prekidnu rutinu timera
void restore();

void sys_dispatch();

#endif
