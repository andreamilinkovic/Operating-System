#include <iostream.h>

#include "pcb.h"

extern int userMain (int argc, char* argv[]);

int retvalue;

int main(int argc, char* argv[]){
	PCB::sys_inic();

	retvalue = userMain(argc, argv);

	PCB::sys_restore();

	return retvalue;
}
