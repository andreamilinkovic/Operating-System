#ifndef _pcb_h_
#define _pcb_h_

#include "thread.h"
#include "queue.h"
#include "idle.h"

class PCB{
public:
	unsigned* stack;
	unsigned ss;
	unsigned sp;
	unsigned bp;

	Time timeSlice;
	StackSize stackSize;

	int finished;
	int blocked;
	int ready;

	int wait_ret; //povratna vrednost na semaforu

	ID id;

	Thread* myThread;

	volatile Time waiting; //vreme cekanja na semaforu

	Queue* waiting_queue; //niti koje cekaju na zavrsetak niti sa ovim PCB-om

	void init_context(); //kreira i inicijalizuje stek

	static ID pcb_id;

	static volatile PCB* running;
	static Thread* initial; //pocetna nit
	static Thread* idle;
	static PCB* pcb_idle;

	static void wrapper(); //f-ja koja okruzuje run metodu

	PCB(Thread* myThread, StackSize stackSize, Time timeSlice);
	PCB();
	~PCB();

	static void sys_inic(); //inicijalizuje sistem
	static void sys_restore(); //vraca sistem na staro
};

#endif
