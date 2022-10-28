#include "_kernelCore.h"
#include "_threadsCore.h"
#include <LPC17xx.h>
#include "osDefs.h"

thread threadList [8];
extern int threadCount; //number of threads created
int osCurrentTask = 0;
uint32_t mspAddr;

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFF << 16;	//sets priority of interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	mspAddr = *MSP_Original; //may not need
	
}

//this function is called by the kernel; it schedules which threads to run
void osSched(void){
	
	if(osCurrentTask >= 0)
	{
		threadList[osCurrentTask].TSP = (uint32_t*)(__get_PSP() - 16*4); //pushes 16 uint32_ts to move the TSP down below garbage registers
	}
	osCurrentTask = (osCurrentTask+1)%(threadCount);
	
	ICSR |= 1<<28;	//changes pendSV state to pending
	__asm("isb");	//tells compiler to run the "isb" instruction using assembly
}//declare globall. copy and plaxe in theadscore and declare at start with extern

bool osKernelStart(){
	
	if(threadCount>0){ //if at least one thread exists
		osCurrentTask=-1; //set osCurrent task to -1 (to set up for osSched method)
		//setThreadingWithPSP(threadList[0].TSP);
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)threadList[0].TSP);
		osSched();
	}
	return 0;
}
	/*Start the kernel, usually by calling a kernel_start type function. This function’s purpose is to
initialize anything that the first thread needs before it gets going (or that the kernel needs
before the first thread runs), then switch to thread mode, switch SP to PSP, and finally start the
27 That will be a HUGE overhead when allocating a new array...
55
first thread. The easiest way to do that is to call the scheduling function, but exactly how you
start threads is a consideration for your OS*/



int task_switch(void){
	__set_PSP((uint32_t)threadList[osCurrentTask].TSP);//set new PSP with TSP
	return 1;
}

