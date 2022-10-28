#ifndef _KERNELCORE
#define _KERNELCORE

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <LPC17xx.h>
#include <stdbool.h>

#include "osDefs.h"

void kernelInit(void);		//initializes memory structures and interrupts necessary to run the kernel
void osSched(void);		//called by the kernel to schedule which threads to run
bool osKernelStart(void);

#define SHPR3 *(uint32_t*)0xE000ED20	//this memory location is pointer to a uint32_t
#define ICSR *(uint32_t*)0xE000ED04	//ICSR is at this memory location

#endif

