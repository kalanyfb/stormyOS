#ifndef OS_DEFS
#define OS_DEFS

//include typedef of threads struct
#include <stdio.h>
#include <stdint.h>

#define THREAD_STACK_SIZE 0x200

typedef struct thread{
	//function pointer
	void (*fun_ptr)(void *args); //takes in a single void pointer
	uint32_t *TSP; //thread stack pointer address of stack
}thread; 


#endif
