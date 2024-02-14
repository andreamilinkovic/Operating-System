#include "queue.h"
#include "pcb.h"
#include "system.h"

Queue::Queue(){
	lock();
	head = tail = 0;
	n = 0;
	unlock();
}

Queue::~Queue(){
	lock();
	Elem* old;
	while(head){
		old = head;
		head = head->next;
		delete old;
	}
	tail = head = 0;
	n = 0;
	unlock();
}

void Queue::put(PCB* pcb){
	lock();
	Elem* elem = new Elem(pcb, 0); //pp da niko nece ubaciti postojeci element u red
	if(head == 0)
		head = tail = elem;
	else
		tail = (tail->next = elem);
	n++;
	unlock();
}

PCB* Queue::get(){
	if(head == 0) return 0;
	lock();
	Elem* old = head;
	PCB* pcb = old->data;

	head = head->next;
	if(head == 0) tail = 0;

	n--;
	delete old;
	unlock();
	return pcb;
}

void Queue::remove(ID id){
	lock();
	Elem* p = head;
	Elem* q = 0;
	while(p){
		if(p->data->id == id){
			if(q == 0)	//prevezivanje elemenata
				head = p->next;
			else
				q->next = p->next;

			if(p == tail) tail = q;
			if(head == 0) tail = 0; //nepotrebno?

			n--;
			delete p;
			break;
		}
		q = p;
		p = p->next;
	}
	unlock();
}

Thread* Queue::find(ID id){
	lock();
	Elem* p = head;
	while(p){
		if(p->data->id == id){
			unlock();
			return p->data->myThread;
		}
		p = p->next;
	}
	unlock();
	return 0;
}

