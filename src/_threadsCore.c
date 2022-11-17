#include "_threadsCore.h"
//#include "_kernelCore.h"
#include <LPC17xx.h>
#include <stdint.h>
#include "stdio.h"
#include "osDefs.h"

extern thread threadList [8];
int threadCount=0;
extern uint32_t OFFSET;
extern uint32_t MAXTHREADS;
extern void osYield(void);

uint32_t* getMSPInitialLocation(void){
	
	//checks vector table location 0x0 for MSP address
	uint32_t* vectorTableStart = (uint32_t*)0x0; 
	//creates the pointer to be used for the MSP from the address in prev line
	uint32_t* MSPInitialLocation = (uint32_t*) *vectorTableStart; 
	
	return MSPInitialLocation; //returns MSP
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t* getNewThreadStack(uint32_t OFFSET){ 
	
	uint32_t* currentMSP = getMSPInitialLocation(); //gets pointer to MSP
	uint32_t MSPAddress = (uint32_t)currentMSP; //address of MSP, casted to 32-bit integer 
	uint32_t PSPAddress = MSPAddress - OFFSET; //subtracts offset to address of MSP
	uint32_t addOffset = 0; //initializes additional offset value
	uint32_t* newPSP; //initializes pointer that will be PSP
	
	if (PSPAddress%8 != 0){ //checks for if divisible by 8
		// if above is false, sets additional offset value to remainder to reach 8
		addOffset = PSPAddress %8; 
	}
	PSPAddress += addOffset; //additional offset is added to PSP address to make it divisible by 8
	newPSP = (uint32_t*)(PSPAddress); // newPSP is set with address that is (correctly) offset
	
	//print PSP address to check
	printf ("PSP address:");
	printf ("%x\n", PSPAddress);
	return newPSP;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setThreadingWithPSP(uint32_t* threadStack){
	
	// set psp with address of threadstack casted to uint_32 refer to given code (1<,1) instead of (2)
	__set_PSP((uint32_t)threadStack);
	__set_CONTROL(2); //set control to 2 to go into thread state
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void osCreateThread(void(*userFunction)(void *args), double freq){
	//pass in function pointer 
	//create new thread stack 
	
	uint32_t patternedValue;
	int i;
	
	if (threadCount<MAXTHREADS)
	{
		if (freq!=0) //checks to see if freq value has been set
		{
			threadList[threadCount].periodic = true; //if freq value is set to a non-zero number then it is periodic
			threadList[threadCount].timerLength = (1/freq)*1000; //freq is converted to period and the timerlength for this thread is set to the period
		}
		else
		{
			threadList[threadCount].periodic = false; //if freq value was set to 0 (ie not set), then it is not periodic
			threadList[threadCount].timerLength = 0; //timer value initialized to 0 and not used
		}
		threadList[threadCount].fun_ptr = userFunction; //sets function pointer to user function pointer
		threadList[threadCount].TSP=getNewThreadStack(MAIN_STACK_SIZE + (threadCount)*THREAD_STACK_SIZE); //set new thread stack pointer
		threadList[threadCount].state = WAITING; //set current state of newly created thread to waiting
		
		//debugging/check
		printf("%x,\n", (int)(threadList[threadCount].TSP)); //print tsp to check
		*(--threadList[threadCount].TSP) = 1<<24;
		printf ("%x\n", *(threadList[threadCount].TSP));
		*(--threadList[threadCount].TSP) = (uint32_t)userFunction;
		//end of check statements
		
		patternedValue=0xA0; //set to a nice value to check in memory easily
		for (i = 0; i<6; i++){
			*(--threadList[threadCount].TSP) = patternedValue; //set memory to given values
			patternedValue = patternedValue + 0x01; //incremement patterned value
		}
		
		patternedValue=0xB0; //set to a nice value to check in memory easily
		for (i = 0; i<8; i++){
			*(--threadList[threadCount].TSP) = patternedValue; //set memory to given values
			patternedValue = patternedValue + 0x01; //incremement patterned value
		}
		threadCount++; //increase threadcount as a new thread has just been created
		printf ("%d\n", threadCount); //print threadcount to check
		
	}
	
}


//should only run if no other options
void osIdleTask(void* args){
	while(1)
	{	
		//for testing- prints "idlethread" but should do nothing when actually implement
		printf("\nidlethread");
		osYield();
	}
}






