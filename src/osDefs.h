#ifndef OS_DEFS
#define OS_DEFS

//include typedef of threads struct
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define THREAD_STACK_SIZE 0x200
#define MAIN_STACK_SIZE 0x200

//thread states
#define IDLE 0
#define ACTIVELY_RUNNING 1 //one thread only
#define WAITING 2
#define SLEEP 3
#define YIELD_SWITCH 0

#define LIST_LENGTH 8
#define FORCE_SWITCH_TIME 100

#define UART_MUTEX_INDEX 0
#define GLOBAL_VAR_MUTEX_INDEX 1
#define LED_MUTEX_INDEX 2

//queue functions
void enqueue(int index);
void dequeue(void);
int peek(void);
void printQueue(void);

extern int waitingQueue[];
extern int front;
extern int rear;
extern int countQueueItems;

typedef struct thread{
	//function pointer
	void (*fun_ptr)(void *args); //takes in a single void pointer
	uint32_t *TSP; //thread stack pointer address of stack
	int state;
	
	int timerLength;
	int napTimer;
}thread; 


typedef struct mutex{ //*note mutex id is from externel list of mutexes
	bool isAvailable; //true means that it can be taken, main part of mutex
	int mutexOwnerThreads[8]; //which threads own it
	int numOwnerThreads; //to count/store indices
	int indexCurrentOwner; //if -1, then no owner
	
} mutex;
	

#endif
