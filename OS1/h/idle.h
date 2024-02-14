#ifndef _idle_h_
#define _idle_h_

class Idle : public Thread{
public:
	Idle() : Thread(defaultStackSize, 1){}

protected:
	void run(){
		while(1){}
	}

};

#endif
