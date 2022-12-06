#include "_kernelCore.h"
#include <LPC17xx.h>
#include "osDefs.h"

//thread array variables
thread threadList [LIST_LENGTH]; //array of all threads
extern int threadCount; //number of threads created

//initialization variables
uint32_t mspAddr; //address of MSP
int osCurrentTask = 0; //index of task currently running

//systick variables
uint32_t timeInThread = 0; //the exact moment (msticks value) when task has switched

//
int pushValue = 8*4; //default value to push if not forced to context switch by timer
int nextState = WAITING; //default state after running (if not being sent to sleep)

//running in sleep state variables/for checks
int numSleepThreads = 0; //value to see if all threads are asleep and therefore idlethread has to run
bool allSleep = 0; //if numSleepThreads == number of threads (idle is treated similarly to sleeping for this check)

//mutex
//bool mutex;

//functions from __threadsCore.c that will be used in __kernelCore.c
extern void osCreateThread(void(*userFunction)(void *args)); 
extern void osIdleTask(void* args);

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFE << 16;	//sets priority of the PENDSV interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	SHPR3 |= 0xFFU << 24; //Set the priority of SysTick to be the weakest
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC the be the strongest
	
	mspAddr = *MSP_Original; //get address of original MSP
	
	osCreateThread(osIdleTask); //idleTask is created in kernelInit so that RTOS users cannot access the idle task
	threadList[0].state = IDLE; //sets state of idle task to the special IDLE state
}

//this function is called by the kernel; it schedules which threads to run
void osYield(void){
	__ASM("SVC #0");//trigger SVC right away
}


bool osKernelStart(){
	
	if(threadCount>1){ //if at least one thread exists besides idlethread
		osCurrentTask=-1; //set osCurrent task to -1 (to set up for osYield method)
		
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)threadList[1].TSP); //run first thread first
		threadList[1].state=ACTIVELY_RUNNING; // for init of kernel, set first thread's state to actively running
		
		osYield();//call osYield to run next thread
	}
	return 0;
}


int task_switch(void){
	__set_PSP((uint32_t)threadList[osCurrentTask].TSP);//set new PSP with TSP
	return 1;
}

void SysTick_Handler(void){
		int i = 0; //count variable initialized
		//SLEEP STATE checking
		for (i=0; i<threadCount; i++) //for loop to iterate and check all threads for sleep condition
		{
			if (threadList[i].timerLength!=0) //a check to ensure that a non-sleep && aperiodic thread does not try to decrement its unused napDLTimer value & overflow
			{
				threadList[i].napTimer--; //decrement timers for all threads that are sleeping &&
			}
			//checks if a thread's state is sleep AND if its naptime is over (wakeup)
			
			if(threadList[i].state == SLEEP && threadList[i].napTimer <=0)
			{
				threadList[i].state = WAITING; //set state to WAITING so that it is ready to be called 
				threadList[i].napTimer=0; //reset napTimer to 0 as it is awake
			}
		}
		
		//FORCED CONTEXT SWITCHING based on running out of time for a thread to run
		timeInThread--;
		
		if (timeInThread <= 0) //if max time elapsed, force context switch
		{
			printf("helloooooooooooooooooooooooooooooooooo");
			pushValue = 8*4; //push 8 registers bc of tail chain condition
			
			if(osCurrentTask >= 0) //checks to ensure that at least one thread exists
			{	
				//pushes (16 or 8) uint32_ts to move the TSP down below garbage registers
				threadList[osCurrentTask].TSP = (uint32_t*)(__get_PSP() - pushValue); 
				if(osCurrentTask==0) //checks if the current running task has an index of one (idleThread)
				{
					nextState = IDLE;//sets its nextState to idle
				}
				//sets the state of currentTask to its nextState for when its called again
				threadList[osCurrentTask].state = nextState; 
				nextState = WAITING; //defaults state back to WAITING
				pushValue = 8*4; //defaults back to 16*4 (ie 16 registers to push)
			}
	
			//increments osCurrentTask (% is to make sure it cycles through 0 to threadCount instead of going above)
			osCurrentTask = (osCurrentTask+1)%(threadCount); 
	
			printf (":))"); //would not run without this ! (race case)
	
			//checks to ensure we do not accidentally run the sleeping thread at all OR the idlethread without them all being asleep
			while((threadList[osCurrentTask].state==SLEEP 
				|| threadList[osCurrentTask].state == IDLE) && allSleep == 0){ 
				if(numSleepThreads==threadCount) //checks for condition of all threads sleeping
				{
					allSleep = 1; //if all sleeping, set bool value to true to exit while loop
					numSleepThreads = 0; // refresh num of sleeping threads to 0 for next iteration
					osCurrentTask = 0; //index of idle thread to run the idle thread
				}
				else
				{
					//defaults to continually increment/look for non-sleeping thread
					osCurrentTask = (osCurrentTask+1)%(threadCount);
				}
				numSleepThreads++;
			}
			allSleep = 0; //resets allSleep to false for next iteration
	
			timeInThread = FORCE_SWITCH_TIME;
			threadList[osCurrentTask].state = ACTIVELY_RUNNING; //sets thread that will run next to ACTIVELY_RUNNING state

			ICSR |= 1<<28;	//changes pendSV state to pending
			__asm("isb");	//tells compiler to run the "isb" instruction using assembly
			
		}
	
}

//sleep function
void osSleep(int time){
	threadList[osCurrentTask].state = SLEEP; //change state of current task to SLEEP
	nextState=SLEEP; //set nextState (for when state is set in osYield) to SLEEP
	threadList[osCurrentTask].timerLength = time; //set naptime var based on given argument
	threadList[osCurrentTask].napTimer = time; //set napt start var to current msTicks value
	
	osYield();//yield to next task
}

//function used to get the value of the system call's immediate
void SVC_Handler_Main(uint32_t *svc_args){
	char call = ((char*)svc_args[6])[-2];
	
	if (call == YIELD_SWITCH){
		if(osCurrentTask >= 0) //checks to ensure that at least one thread exists
		{	
			//pushes (16 or 8) uint32_ts to move the TSP down below garbage registers
			threadList[osCurrentTask].TSP = (uint32_t*)(__get_PSP() - pushValue); 
			if(osCurrentTask==0) //checks if the current running task has an index of one (idleThread)
			{
				nextState = IDLE;//sets its nextState to idle
			}
			//sets the state of currentTask to its nextState for when its called again
			threadList[osCurrentTask].state = nextState; 
			nextState = WAITING; //defaults state back to WAITING
			pushValue = 8*4; //defaults back to 16*4 (ie 16 registers to push)
		}
	
		//increments osCurrentTask (% is to make sure it cycles through 0 to threadCount instead of going above)
		osCurrentTask = (osCurrentTask+1)%(threadCount); 
	
		printf (":))"); //would not run without this ! (race case)
	
		//checks to ensure we do not accidentally run the sleeping thread at all OR the idlethread without them all being asleep
		while((threadList[osCurrentTask].state==SLEEP 
			|| threadList[osCurrentTask].state == IDLE) && allSleep == 0){ 
			if(numSleepThreads==threadCount) //checks for condition of all threads sleeping
			{
				allSleep = 1; //if all sleeping, set bool value to true to exit while loop
				numSleepThreads = 0; // refresh num of sleeping threads to 0 for next iteration
				osCurrentTask = 0; //index of idle thread to run the idle thread
			}
			else
			{
				//defaults to continually increment/look for non-sleeping thread
				osCurrentTask = (osCurrentTask+1)%(threadCount);
			}
			numSleepThreads++;
		}
		allSleep = 0; //resets allSleep to false for next iteration
	
		timeInThread = FORCE_SWITCH_TIME;
		threadList[osCurrentTask].state = ACTIVELY_RUNNING; //sets thread that will run next to ACTIVELY_RUNNING state

		ICSR |= 1<<28;	//changes pendSV state to pending
		__asm("isb");	//tells compiler to run the "isb" instruction using assembly
	}
}


