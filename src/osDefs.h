//include typedef of threads struct
#include <stdio.h>
#include <stdint.h>



typedef struct thread{
	//function pointer
	void (*fun_ptr)(void *args); //takes in a single void pointer
	uint32_t *TSP; //thread stack pointer address of stack
}thread; 