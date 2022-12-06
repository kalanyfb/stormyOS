//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

#include <stdbool.h>

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"
#include "_mutexCore.h"



uint32_t OFFSET=512;
uint32_t MAXTHREADS=8;


void threadOne (void *args){
	bool run1=0;
	run1=osMutexAcquire(0);
	if(run1 == true)
	{
		while(1)
		{
			printf("\n thread 1************************************************* ");
			printf("%d\n", osMutexRelease(0));
			osYield();
		}
	}
	else
	{
		osYield();
	}
}

void threadTwo (void *args){
	bool run2=0;
	run2=osMutexAcquire(0);
	if(run2 == true)
	{
		while(1)
		{
			printf("\n thread 2_________________________________________________ ");
			printf("!!!!!");
			printf("%d\n", osMutexRelease(0));
			osYield();
		}
	}
	else
	{
		osYield();
	}
}

void threadThree (void *args){
	bool run3=0;
	run3=osMutexAcquire(0);
	if(run3 == true)
	{
		while(1)
		{
			printf("\n thread 2_________________________________________________ ");
			printf("%d\n", osMutexRelease(0));
			osYield();
		}
	}
	else
	{
		osYield();
	}
}


int main( void ) 
{
	uint32_t* MSPPtr;
	int indexThreadOne = 0;
	int indexThreadTwo = 0;
	int indexThreadThree = 0;
	int indexMutexOne;
	int ownThreads[8];
	int nOwnThreads;
	SystemInit(); 
	
	printf("\n hello world"); 
	MSPPtr = getMSPInitialLocation();
	printf("\n MSP PTR: ");
	printf ("%x\n", (int)&MSPPtr);
	
	
	//initialize kernel settings/vars
	kernelInit();
	
	
	//initialize threads with functions at top of file
	indexThreadOne = osCreateThread(threadOne);
	indexThreadTwo = osCreateThread(threadTwo);
	indexThreadThree = osCreateThread(threadThree);
	
	SysTick_Config(SystemCoreClock/1000); //calls systick_handler every ms
	
	
	ownThreads[0] = indexThreadOne;
	ownThreads[1] = indexThreadTwo;
	ownThreads[2] = indexThreadThree;
	nOwnThreads = 3;
	indexMutexOne = osCreateMutex(nOwnThreads, ownThreads);
	
	printf("\n index of mutex 1: ");
	printf("%d", indexMutexOne);
	//start kernel
	osKernelStart();
	
	while (1);
}








