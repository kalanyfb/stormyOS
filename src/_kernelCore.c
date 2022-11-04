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
volatile uint32_t msTicks = 0; //determines time passed since start of program
bool taskSwitched = 0; //boolean to check if task has just switched (ie if one thread has switched to another)
uint32_t timeInThread = 0; //the exact moment (msticks value) when task has switched

//
int pushValue = 16*4; //default value to push if not forced to context switch by timer
int nextState = WAITING; //default state after running (if not being sent to sleep)

//running in sleep state variables/for checks
int numSleepThreads = 0; //value to see if all threads are asleep and therefore idlethread has to run
bool allSleep = 0; //if numSleepThreads == number of threads (idle is treated similarly to sleeping for this check)

//mutex
bool mutex;

//functions from __threadsCore.c that will be used in __kernelCore.c
extern void osCreateThread(void(*userFunction)(void *args)); 
extern void osIdleTask(void* args);

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFF << 16;	//sets priority of interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	mspAddr = *MSP_Original; //get address of original MSP
	
	osCreateThread(osIdleTask); //idleTask is created in kernelInit so that RTOS users cannot access the idle task
	threadList[0].state = IDLE; //sets state of idle task to the special IDLE state
}

//this function is called by the kernel; it schedules which threads to run
void osYield(void){
	mutex = false; //sets mutex to false to ensure that systick cannot do anything while yield occurs
	
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
		pushValue = 16*4; //defaults back to 16*4 (ie 16 registers to push)
	}
	
	//increments osCurrentTask (% is to make sure it cycles through 0 to threadCount instead of going above)
	osCurrentTask = (osCurrentTask+1)%(threadCount); 
	
	printf (":))"); //would not run without this ! (race case)
	
	//checks to ensure we do not accidentally run the sleeping thread at all OR the idlethread without them all being asleep
	while((threadList[osCurrentTask].state==SLEEP 
		|| threadList[osCurrentTask].state == IDLE) && allSleep == 0){ 
		numSleepThreads++;
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
	}
	allSleep = 0; //resets allSleep to false for next iteration
	
	taskSwitched = 1; //sets taskSwitched to true to ensure that systick starts timer for this thread now
	threadList[osCurrentTask].state = ACTIVELY_RUNNING; //sets thread that will run next to ACTIVELY_RUNNING state
	mutex = true; //sets mutex to true to allow systick to run
	ICSR |= 1<<28;	//changes pendSV state to pending
	__asm("isb");	//tells compiler to run the "isb" instruction using assembly
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
	if(mutex==true){ //checks if mutex == true to use systick handler
		int i = 0; //count variable initialized
		int timeElapsed = 0; //time elapsed reset to 0
		msTicks++; //increase msTicks (acts like a stopwatch)
	
		//SLEEP STATE checking
		for (i=0; i<threadCount; i++) //for loop to iterate and check all threads for sleep condition
		{
			//checks if a thread's state is sleep AND if its naptime is over (wakeup)
			if(threadList[i].state == SLEEP 
				&& (msTicks-threadList[i].napStart)>=threadList[i].napLength) 
			{
				threadList[i].state = WAITING; //set state to WAITING so that it is ready to be called 
				threadList[i].napStart=0; //reset napSTART
			}
		}
		
		//FORCED CONTEXT SWITCHING checking
		if(taskSwitched) //checks if task has just switched
		{
			taskSwitched = 0; //inits vars, sets taskSwitched to 0 so it cant be called until the task has switched again
			timeInThread = msTicks; //current msTicks stored in timeInThread
		}
		timeElapsed = msTicks-timeInThread; //time elapsed from the last task switch is constantly calculated
		
		if ((timeElapsed-FORCE_SWITCH_TIME) >= 0 && taskSwitched==0) //if the max time has elapsed, then force a context switch
		{
			printf("helloooooooooooooooooooooooooooooooooo");
			pushValue = 8*4; //push 8 registers bc of tail chain condition
			osYield(); //force osYield
		}
	}
}

//sleep function
void osSleep(int time){
	threadList[osCurrentTask].state = SLEEP; //change state of current task to SLEEP
	nextState=SLEEP; //set nextState (for when state is set in osYield) to SLEEP
	threadList[osCurrentTask].napLength = time; //set naptime var based on given argument
	threadList[osCurrentTask].napStart = msTicks; //set napt start var to current msTicks value
	
	osYield();//yield to next task
}


