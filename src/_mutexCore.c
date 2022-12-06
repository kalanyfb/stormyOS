#include <LPC17xx.h>
#include <stdint.h>
#include "stdio.h"
#include "osDefs.h"
#include "_mutexCore.h"

mutex mutexList [8];
int mutexCount=0;
uint32_t MAX_MUTEXES = 8;
extern int osCurrentTask;
extern thread threadList[];
extern int nextState;

bool osMutexAcquire(int mutexIndex){
	//if no thread currently has acquired the mutex, then the thrad asking to acquire the mutex 
	//is given access, function returns
	//mutex becomes unavailable
	//thread must specify which mutex it wants to acquire and how long it is willing to wait 
	//bool return true if successful
	//if unavilable, add to waiting queue - if it enters the waiting queue, CONTEXT SWITCH
	//waiting threads are not reawakened unless timeout or mutex released
	int i=0;
	
	while(mutexList[mutexIndex].mutexOwnerThreads[i]!=osCurrentTask)
	{
			i++;
		if(i>=mutexList[mutexIndex].numOwnerThreads-1)
		{
			//attempted to acquire mutex it does not own context switch
			return false;
		}
	}
	if(mutexList[mutexIndex].isAvailable == 1)
	{
		mutexList[mutexIndex].isAvailable = 0;
		mutexList[mutexIndex].indexCurrentOwner = osCurrentTask;
		return true;
	}
	else if (mutexList[mutexIndex].indexCurrentOwner == osCurrentTask)
	{
		//already acquired
		return true;
	}
	else
	{
		//it is an owner, but it's not available
		enqueue(osCurrentTask);
		nextState = SLEEP;
		//nextSTATE IS SLEEP
		return false;
	}
}

bool osMutexRelease(int mutexIndex){
	//thread attempts to release mutex 
	//if it has ownership of mutex, mutex is released and the OS is free to 
	//schedule another thread in waiting queue
	//not put to sleep, may continue to run until pre-empted or co-op context siwtch occurs
	//HOW WILL NEXT TASK BE SCHEDULED : give mutex to next thread in waiting queue and put into ready queue
	//then round robin scheduler
	if(mutexList[mutexIndex].indexCurrentOwner == osCurrentTask)
	{
		//release mutex
		mutexList[mutexIndex].isAvailable = 1;
		//check if there is one next in queue
		if(front != -1)
		{
			//nextState = READY -> oscurrenttask = front, osAcquire
			threadList[front].state = WAITING;
		}
		return 1; //release was succes
	}
	
	
	//once released check if there is a thread in the waiting queue - only in scheduler/whemn time to run a new thread
	//once released may not be put to sleep
	
	return 0;
}

int osCreateMutex(int nOwnerThreads, int ownerThreads[]){
	//create mutex - often called before kernel runs 
	//what does this function need to run (arguments)
	//return an ID for threads to acquire the mutex
	//should: allocate memory, set up mutex according to design, make it available to OS to assign ownership
	int i;
	
	if(mutexCount < MAX_MUTEXES)
	{
		mutexList[mutexCount].isAvailable = true;
		for(i=0; i<nOwnerThreads; i++)
		{
			mutexList[mutexCount].mutexOwnerThreads[i] = ownerThreads[i];
		}
		mutexList[mutexCount].numOwnerThreads = nOwnerThreads;
		mutexCount++; 
	}
	return (mutexCount-1);
}

