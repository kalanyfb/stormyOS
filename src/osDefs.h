//include typedef of threads struct
#include <stdio.h>
#include <stdint.h>



typedef struct thread{
	//function pointer
	void (*fun_ptr)(void *args);
	uint32_t *TSP; //thread stack pointer
}thread; 