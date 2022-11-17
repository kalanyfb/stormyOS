//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

#include <stdbool.h>

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

uint32_t OFFSET=512;
uint32_t MAXTHREADS=8;


void threadOne (void *args){
	while(1)
	{
		printf("\n thread1************");
		//osYield();
		osSleep(70);
	}
}

void threadTwo (void *args){
	while(1)
	{
		printf("\n thread2____________");
		osSleep(8);
		//osYield();
	}
}

void threadThree (void *args){
	while(1)
	{
		printf("\n thread 3                     &&&&                    &&&&       ");
	}
}

void threadFour (void *args){
	while(1)
	{
		printf("\n thread4++++++++++++ ");
		//osSleep(700);
		osYield();
	}
}

/*
void osIdleTask(void* args){
	while(1)
	{
		printf("\n default thread");
		//osYield();
	}
}*/

int main( void ) 
{
	uint32_t* MSPPtr;
	SystemInit(); 
	
	printf("\n hello world"); 
	MSPPtr = getMSPInitialLocation();
	printf("\n MSP PTR: ");
	printf ("%x\n", (int)&MSPPtr);
	
	//initialize kernel settings/vars
	kernelInit();
	
	//initialize threads with functions at top of file
	//test case set 1
	osCreateThread(threadOne,256); //function threadOne with freq of 256
	osCreateThread(threadTwo,100); //function threadTwo with freq of 100
	osCreateThread(threadFour,12); //function threadFour with freq of 12
	
	//moved systick_config so that the handler wouldn't be called before ready
	SysTick_Config(SystemCoreClock/1000); //calls systick_handler every ms
	
	//start kernel
	osKernelStart();

	while (1);
}








