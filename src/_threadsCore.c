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

extern bool kernelStarted;
extern void osYield(void);

uint32_t* getMSPInitialLocation(void){
	
	uint32_t* vectorTableStart = (uint32_t*)0x0; //checks vector table location 0x0 for MSP address
	uint32_t* MSPInitialLocation = (uint32_t*) *vectorTableStart; //creates the pointer to be used for the MSP from the address in prev line
	
	return MSPInitialLocation; //returns MSP
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t* getNewThreadStack(uint32_t OFFSET){ // can i get a new stack, include error checking later(???)
	
	//should i keep the msp value as a global variable? or just the current MSP?\
	//should decrement msp
	
	uint32_t* currentMSP = getMSPInitialLocation(); //gets pointer to MSP
	uint32_t MSPAddress = (uint32_t)currentMSP; //address of MSP, casted to 32-bit integer 
	uint32_t PSPAddress = MSPAddress - OFFSET; //subtracts offset to address of MSP
	uint32_t addOffset = 0; //initializes additional offset value
	uint32_t* newPSP; //initializes pointer that will be PSP
	
	if (PSPAddress%8 != 0){ //checks for if divisible by 8
		addOffset = PSPAddress %8; // if above is false, sets additional offset value to remainder to reach 8
	}
	PSPAddress += addOffset; //additional offset is added to PSP address to make it divisible by 8
	newPSP = (uint32_t*)(PSPAddress); // newPSP is set with address that is (correctly) offset
	printf ("PSP address:");
	
	printf ("%x\n", PSPAddress);
	return newPSP;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setThreadingWithPSP(uint32_t* threadStack){
	
	__set_PSP((uint32_t)threadStack);// set psp with address of threadstack casted to uint_32 refer to given code (1<,1) instead of (2)
	__set_CONTROL(2); //set control to 2 to go into thread state
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void osCreateThread(void(*userFunction)(void *args)){
	//pass in function pointer 
	//create new thread stack 
	
	uint32_t patternedValue;
	int i;
	
	if (threadCount<MAXTHREADS)
	{
		threadList[threadCount].fun_ptr = userFunction;
		threadList[threadCount].TSP=getNewThreadStack(MAIN_STACK_SIZE + (threadCount)*THREAD_STACK_SIZE); //AAA
		
		printf("%x,\n", (int)(threadList[threadCount].TSP));
		*(--threadList[threadCount].TSP) = 1<<24;
		printf ("%x\n", *(threadList[threadCount].TSP));
		*(--threadList[threadCount].TSP) = (uint32_t)userFunction;
		//printf ("%d\n", (uint32_t)userFunction);
		//printf("%d\n", *(threadList[threadCount].TSP));
		
		patternedValue=0xA0;
		for (i = 0; i<6; i++){
			*(--threadList[threadCount].TSP) = patternedValue;
			patternedValue = patternedValue + 0x01;
		}
		
		patternedValue=0xB0;
		for (i = 0; i<8; i++){
			*(--threadList[threadCount].TSP) = patternedValue;
			patternedValue = patternedValue + 0x01;
		}
		threadCount++;
		printf ("%d\n", threadCount);
		
	}
	
}

//should only run if no other options
void osIdleTask(void* args){
	while(1)
	{
		printf("\n idlethread");
		printf ("%d\n", kernelStarted);
		osYield();
	}
}






