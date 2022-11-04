#include <stdint.h>
#include <LPC17xx.h>

#ifndef THREADS_CORE_H
#define THREADS_CORE_H

uint32_t* getMSPInitialLocation(void); //Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getNewThreadStack(uint32_t offset); //Returns the address ofa new PSP with offset of “offset” bytes from MSP. Be careful withpointer arithmetic! It’s best to cast to an integer then back if you’renot sure.
void setThreadingWithPSP(uint32_t* threadStack); //Sets the value ofPSP to threadStack and ensures that the microcontroller is using thatvalue by changing the CONTROL register
void osCreateThread(void(*userFunction)(void *args)); //creates thread and puts it in array, inits stack values
void osIdleTask(void* args); //idle task function
#endif
