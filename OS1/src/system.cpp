#include <iostream.h>
#include <dos.h>

#include "SCHEDULE.H"
#include "pcb.h"
#include "system.h"

volatile unsigned int lockFlag = 1; //zabrana promene konteksta bez zabrane prekida; 1-dozvoljeno

Queue* threads = new Queue();
KernelSem* semaphores[256]; //max 256 semafora
IVTEntry* entries[256];

//pomocne promenljive za prekid tajmera
unsigned tsp;
unsigned tss;
unsigned tbp;

volatile int brojac = 20;
volatile int zahtevana_promena_konteksta = 0;

void interrupt timer(){	// prekidna rutina
	if (!zahtevana_promena_konteksta) {
		brojac--;
		tick();
		//provera da li nit na semaforu treba da se odblokira
		for(int i = 0; i < KernelSem::sem_id; i++){
			if(semaphores[i] != 0){
				KernelSem* sem = semaphores[i];
				Elem* cur = sem->blocked_queue->head;
				while(cur != 0){
					if(--cur->data->waiting == 0){
						PCB* pcb = cur->data;
						pcb->blocked = 0;
						pcb->ready = 1;
						pcb->wait_ret = 0;
						sem->value++;
						Scheduler::put(pcb);
						Elem* old = cur;
						cur = cur->next;
						sem->blocked_queue->remove(old->data->id);
					}
					else
						 cur = cur->next;
				}
			}
		}
		asm int 60h;
	}

	if (brojac == 0 || zahtevana_promena_konteksta) {
		if(lockFlag){
			zahtevana_promena_konteksta = 0;

			asm {
				// cuva sp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}

			PCB::running->sp = tsp;
			PCB::running->ss = tss;
			PCB::running->bp = tbp;

			if(PCB::running->ready && PCB::running != PCB::pcb_idle)
				Scheduler::put((PCB*)PCB::running);	// Scheduler
			PCB::running = Scheduler::get();
			if(PCB::running == 0)
				PCB::running = PCB::pcb_idle;

			tsp = PCB::running->sp;
			tss = PCB::running->ss;
			tbp = PCB::running->bp;

			brojac = PCB::running->timeSlice;

			asm {
				mov sp, tsp   // restore sp
				mov ss, tss
				mov bp, tbp;
			}
		}
		else zahtevana_promena_konteksta = 1;
	}
}

unsigned oldTimerOFF, oldTimerSEG; // stara prekidna rutina

// postavlja novu prekidnu rutinu
void inic(){
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

// vraca staru prekidnu rutinu
void restore(){
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax


		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}

void sys_dispatch(){
	asm cli;
	zahtevana_promena_konteksta = 1;
	timer();
	asm sti;
}
