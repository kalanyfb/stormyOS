//This file contains relevant pin and other settings 
#include <LPC17xx.h>

//This file is for printf and other IO functions
#include "stdio.h"

//this file sets up the UART
#include "uart.h"

#include "_threadsCore.h"
#include "_kernelCore.h"

void threadOne (void *args){
	while(1)
	{
		printf("thread 1 ");
		osSched();
	}
}

void threadTwo (void *args){
	while(1)
	{
		printf("thread 2 ");
		osSched();
	}
}

void osIdleTask(void* args)
{
	while(1)
	{
		printf("default thread");
		osSched();
	}
}

int main( void ) 
{
	/*
	//Always call this function at the start. It sets up various peripherals, the clock etc. If you don't call this
	//you may see some weird behaviour
	uint32_t* MSPPtr;//declaration of main stack pointer
	uint32_t* PSPPtr;//declaration of process stack pointer
	uint32_t offset = 512; //declaration of variable for offset, val is 512 per lab manual
	
	
	SystemInit();
	
	printf("\n hello world"); //debug statement, earlier code segment
	
	//below finds and sets PSP value and moves into thread state
	MSPPtr = getMSPInitialLocation(); // store MSP pointer in MSPPtr var
	//PSPPtr = getNewThreadStack(offset); //create PSPPtr offset by prev val from MSPPtr
	//setThreadingWithPSP(PSPPtr); //sets PSPPtr as the PSP pointer (in register), sets to Thread mode
	
	
	//priority and scheduling
	kernelInit(); //init memory structures and priority of interrupts necessary to run kernel
								// currently only controlling priority of PendSV
	osSched(); // called by kernel to schedule which threads to run
	*/
	
	//newStart
	uint32_t* MSPPtr;
	uint32_t offset=512;
	SystemInit(); //should go first??
	
	printf("\n hello world"); 
	MSPPtr = getMSPInitialLocation();
	printf("\n MSP PTR: ");
	printf ("%d", (int)&MSPPtr);
	
	kernelInit();
	
	osCreateThread(osIdleTask);
	osCreateThread(threadOne);
	osCreateThread(threadTwo);
	
	//osKernelStart();
	
	
	
	while (1);
}




/*

void turnOn(unsigned int leds) {
	int led0 = 1;
	int led1 = 2;
	int led2 = 4;
	int led3 = 8;
	int led4 = 16;
	int led5 = 32;
	int led6 = 64;
	int led7 = 128;
	
	if (leds & led0) {
		LPC_GPIO1 -> FIOSET |= 1U << 28;
	}
	if (leds & led1) {
		LPC_GPIO1 -> FIOSET |= 1U << 29;
	}
	if (leds & led2) {
		LPC_GPIO1 -> FIOSET |= 1U << 31;
	}
	if (leds & led3) {
		LPC_GPIO2 -> FIOSET |= 1U << 2;
	}
	if (leds & led4) {
		LPC_GPIO2 -> FIOSET |= 1U << 3;
	}
	if (leds & led5) {
		LPC_GPIO2 -> FIOSET |= 1U << 4;
	}
	if (leds & led6) {
		LPC_GPIO2 -> FIOSET |= 1U << 5;
	}
	if (leds & led7) {
		LPC_GPIO2 -> FIOSET |= 1U << 6;
	}
}



void read (void){
	unsigned int left = 0x800000;
	unsigned int right = 0x2000000; 
	unsigned int up = 0x1000000;
	unsigned int down = 0x4000000; 
	unsigned int press = 1048576; 
	unsigned int btn = 1024;
	
	if (!(LPC_GPIO1->FIOPIN & left))
		printf("left\n");
	if (!(LPC_GPIO1->FIOPIN & right))
		printf("right\n");
	if (!(LPC_GPIO1->FIOPIN & up))
		printf("up\n");
	if (!(LPC_GPIO1->FIOPIN & down))
		printf("down\n");
	if (!(LPC_GPIO1->FIOPIN & press))
		printf("press\n");
	if (!(LPC_GPIO2->FIOPIN & btn))
		printf("btn\n");
}	



//This is C. The expected function heading is int main(void)
int main( void ) 
{
	//Always call this function at the start. It sets up various peripherals, the clock etc. If you don't call this
	//you may see some weird behaviour
	uint32_t* MSPPtr;
	uint32_t* PSPPtr;
	uint32_t offset;
	SystemInit();
	
	
	printf("\n hello world");
	
	
	
	MSPPtr = getMSPInitialLocation();
	printf ("\n MSP: ");
	printf ("%d", (int)&MSPPtr);
	
	offset = 512;
	PSPPtr = getNewThreadStack(offset);
	//printf ("\n 3 ");
	//printf ("%d", (int)&PSPPtr);
	
	setThreadingWithPSP(PSPPtr);
	//printf ("\n PSP: ");
	//printf ("%d", (int)&PSPPtr);
	
	//setThreadingwithPSP(PSPPtr);
	
	kernelInit();
	osSched();
	
	while (1);
	
	//Printf now goes to the UART, so be sure to have PuTTY open and connected
	
		
	
	//Your code should always terminate in an endless loop if it is done. If you don't
	//the processor will enter a hardfault and will be weird
	
	int size = 1;
	while(1) {
		unsigned int up = 0x1000000;
		unsigned int down = 0x4000000;
		unsigned int press = 1048576; 
		
		if (!(LPC_GPIO1->FIOPIN & up)){
			size++;
		}
		if (!(LPC_GPIO1->FIOPIN & down)){
			if (size <2)
				size--;
		}
		printf ("size: ");
		printf ("%d", size);
		printf("\n");
		if (!(LPC_GPIO1->FIOPIN & press)){
			int* array = malloc(size*sizeof(int));
			for (int i = 1; i<=size; i++)
			{	
				array[i]=i;
				printf ("%d", i);
				printf("\n");
			}
			free(array);
			size = 1;
		}
	}
	*/



