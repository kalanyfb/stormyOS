#include "_threadsCore.h"
#include <LPC17xx.h>
#include <stdint.h>
#include "stdio.h"
#include "osDefs.h"

extern thread threadList [8];
int arrayCount=0;

uint32_t* getMSPInitialLocation(void){
	
	uint32_t* vectorTableStart = (uint32_t*)0x0; //checks vector table location 0x0 for MSP address
	uint32_t* MSPInitialLocation = (uint32_t*) *vectorTableStart; //creates the pointer to be used for the MSP from the address in prev line
	
	return MSPInitialLocation; //returns MSP
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t* getNewThreadStack(uint32_t offset){ // can i get a new stack, include error checking later(???)
	
	//should i keep the msp value as a global variable? or just the current MSP?\
	//should decrement msp
	
	uint32_t* currentMSP = getMSPInitialLocation(); //gets pointer to MSP
	uint32_t MSPAddress = (uint32_t)currentMSP; //address of MSP, casted to 32-bit integer 
	uint32_t PSPAddress = MSPAddress - offset; //subtracts offset to address of MSP
	uint32_t addOffset = 0; //initializes additional offset value
	uint32_t* newPSP; //initializes pointer that will be PSP
	
	if (PSPAddress%8 != 0){ //checks for if divisible by 8
		addOffset = PSPAddress %8; // if above is false, sets additional offset value to remainder to reach 8
	}
	PSPAddress += addOffset; //additional offset is added to PSP address to make it divisible by 8
	newPSP = (uint32_t*)(PSPAddress); // newPSP is set with address that is (correctly) offset
	
	return newPSP;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setThreadingWithPSP(uint32_t* threadStack){
	
	__set_PSP((uint32_t)threadStack);// set psp with address of threadstack casted to uint_32 refer to given code (1<,1) instead of (2)
	__set_CONTROL(2); //set control to 2 to go into thread state
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void osCreatethread(void (*userFunction)){
	//pass in function pointer 
	//create new thread stack 
	uint32_t* threadPointer;
	threadPointer = getNewThreadStack ((uint32_t)512);
	//pass in values to struct
	thread newThread = (thread){.fun_ptr = userFunction, .TSP = threadPointer};
	//pass in struct to array - parse array to look for first open spot (???)
	//for(int count = 0; count < 8; count++){
	//	if(threadList[count] == NULL)
	//		threadList[count] = newThread;
	threadList[arrayCount] = newThread;
	arrayCount++;
}

void osKernelStart()/////







