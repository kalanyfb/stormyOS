#include "_kernelCore.h"
#include "_threadsCore.h"
#include <LPC17xx.h>
#include "osDefs.h"

thread threadList [8];
thread sleepList [8];
extern int threadCount; //number of threads created
int sleepCount = 0;
int osCurrentTask = 0;
uint32_t mspAddr;
volatile uint32_t msTicks = 0;


//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFF << 16;	//sets priority of interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	mspAddr = *MSP_Original; //may not need
	
}

//this function is called by the kernel; it schedules which threads to run
void osYield(void){
	
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
		osCurrentTask=-1; //set osCurrent task to -1 (to set up for osYield method)
		//setThreadingWithPSP(threadList[0].TSP);
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)threadList[0].TSP);
		
		osYield();
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

void SysTick_Handler(void){
	msTicks++;
	int i;
	
	for (i=0; i<(sleepCount-1); i++)
	{
		if(sleepList[i].state == SLEEP 
			&& (msTicks-sleepList[i].napStart)%sleepList[i].napLength==0)
		{
			sleepList[i].state = WAITING;
		}
	}
	if(threadList[osCurrentTask].state ==  SLEEP)
	{
		//record position, consistently check for if time is up for each of sleepuign threads in array, then change state to wait
	}
	//if(msTicks%1000==0) //modulus amount of time for each thread
	//{
		//osYield();	
	//}
	
	//if thread currently being run.state=sleep and .time is up
	
}

void osSleep(int time){
	threadList[osCurrentTask].state = SLEEP;
	sleepList[sleepCount]=threadList[osCurrentTask]; //add to sleep array
	sleepList[sleepCount].napLength = time;
	sleepList[sleepCount].napStart = msTicks;
	
}


