#include <stdint.h>
#include <LPC17xx.h>

#ifndef THREADS_CORE_H
#define THREADS_CORE_H

//Obtains the initial location of MSP by looking it up in the vector table
uint32_t* getMSPInitialLocation(void); 

//Returns address of new PSP with offset of “offset” bytes from MSP. 
uint32_t* getNewThreadStack(uint32_t offset); 

//Sets value of PSP to threadStack & ensures microcontroller is using that value by changing CONTROL register
void setThreadingWithPSP(uint32_t* threadStack); 

//creates thread and puts it in array, inits stack values - for lab 4 added frequency as an argument for periodic threads
void osCreateThread(void(*userFunction)(void *args), double freq); 

//idle task function
void osIdleTask(void* args); 

#endif
