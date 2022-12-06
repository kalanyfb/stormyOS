#ifndef _MUTEXCORE
#define _MUTEXCORE

#include <LPC17xx.h>
#include <stdint.h>
#include "stdio.h"
#include "osDefs.h"


//function declarations
bool osMutexAcquire(int mutexIndex);
bool osMutexRelease(int mutexIndex);
int osCreateMutex(int nOwnerThreads, int ownerThreads[]);
extern int waitingQueue[];


#endif
