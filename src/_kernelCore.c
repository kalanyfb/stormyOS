#include "_kernelCore.h"
#include <LPC17xx.h>
#include "osDefs.h"

thread threadList [8];

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	SHPR3 |= 0xFF << 16;	//sets priority of interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
}

//this function is called by the kernel; it schedules which threads to run
void osSched(void){
	ICSR |= 1<<28;	//changes pendSV state to pending
	__asm("isb");	//tells compiler to run the "isb" instruction using assembly
}//declare globall. copy and plaxe in theadscore and declare at start with extern

void osKernelStart(){
	//  set initial thread
	//get psp and msp right
	//set threading with psp that is created 
	//then switch into thread
}

int task_switch(void){
	//set new PSP with TSP
	__set_PSP((uint32_t)threadList[]//how to get osCurretnstatus)
}