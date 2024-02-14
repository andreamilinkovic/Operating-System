#ifndef _queue_h_
#define _queue_h_

#include "thread.h"

struct Elem{
	PCB* data;
	Elem* next;
	Elem(PCB* pcb, Elem* n):data(pcb), next(n){}
};

class Queue{
public:
	Elem *head, *tail;

	int n; //br elem u listi

	Queue();

	~Queue();

	void put(PCB* pcb); //stavlja na kraj

	PCB* get(); //uzima prvi element iz liste

	void remove(ID id); //izbacuje iz liste

	Thread* find(ID id); //vraca nit iz liste
};

#endif
