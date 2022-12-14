#ifndef _KERNELCORE
#define _KERNELCORE

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <LPC17xx.h>
#include <stdbool.h>

//#include "osDefs.h"

void kernelInit(void);		//initializes memory structures and interrupts necessary to run the kernel
void osYield(void);		//called by the kernel to schedule which threads to run
bool osKernelStart(void);
void SysTick_Handler(void);
void osSleep(int time);
void osSched(void);

#define SHPR3 *(uint32_t*)0xE000ED20	//this memory location is pointer to a uint32_t
#define SHPR2 *(uint32_t*)0xE000ED1C //stores SVC's priority

#define ICSR *(uint32_t*)0xE000ED04	//ICSR is at this memory location

#endif

