#include "_kernelCore.h"
//#include "_threadsCore.h"
//#include "p1_main.c"
#include <LPC17xx.h>
#include "osDefs.h"

thread threadList [LIST_LENGTH];
//thread sleepList [8];
//thread waitList[8];

extern int threadCount; //number of threads created
//int sleepCount = 0;
int osCurrentTask = 0;
uint32_t mspAddr;
volatile uint32_t msTicks = 0;
int storeTask = -1; //replace with boolean
bool taskSwitched = 0;
uint32_t timeInThread = 0;
extern bool kernelStarted;
int pushValue = 16*4;
int nextState = WAITING;
int numSleepThreads = 0;
bool allSleep = 0;
bool mutex;

extern void osCreateThread(void(*userFunction)(void *args));
extern void osIdleTask(void* args);

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFF << 16;	//sets priority of interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	mspAddr = *MSP_Original; //get address of original MSP
	
	//AAA
	osCreateThread(osIdleTask);
	threadList[0].state = IDLE;
	
}

//this function is called by the kernel; it schedules which threads to run
void osYield(void){
	mutex = false;
	taskSwitched = 1;
	//printf ("\n taskswitched: ");
	//printf("%d\n",taskSwitched);
	
	
	
	if(osCurrentTask >= 0)
	{
		threadList[osCurrentTask].TSP = (uint32_t*)(__get_PSP() - pushValue); //pushes 16 uint32_ts to move the TSP down below garbage registers
		if(osCurrentTask==0)
		{
			nextState = IDLE;
		}
		threadList[osCurrentTask].state = nextState; 
		nextState = WAITING;
		pushValue = 16*4;
	}
	
	
	osCurrentTask = (osCurrentTask+1)%(threadCount);
	
	printf (":))");
	
	while((threadList[osCurrentTask].state==SLEEP 
		|| threadList[osCurrentTask].state == IDLE) && allSleep == 0){
		//printf ("here");
		numSleepThreads++;
		//printf("numSleepThreads");
		//printf("%d\n", numSleepThreads);
		if(numSleepThreads==threadCount)
		{
			allSleep = 1;
			numSleepThreads = 0; //refresh
			osCurrentTask = 0; //index of idle thread
		}
		else
		{
			osCurrentTask = (osCurrentTask+1)%(threadCount);
		}
	}
	allSleep = 0;
	
	threadList[osCurrentTask].state = ACTIVELY_RUNNING;
	mutex = true;
	ICSR |= 1<<28;	//changes pendSV state to pending
	__asm("isb");	//tells compiler to run the "isb" instruction using assembly
	
}//declare global. copy and plaxe in theadscore and declare at start with extern





bool osKernelStart(){
	
	if(threadCount>0){ //if at least one thread exists
		osCurrentTask=-1; //set osCurrent task to -1 (to set up for osYield method)
		//setThreadingWithPSP(threadList[0].TSP);
		__set_CONTROL(1<<1);
		__set_PSP((uint32_t)threadList[0].TSP);
		threadList[0].state=ACTIVELY_RUNNING;
		
		osYield();
	}
	return 0;
}


int task_switch(void){
	//printf("\n osCurrentTask");
	//printf ("%d\n", osCurrentTask);
	__set_PSP((uint32_t)threadList[osCurrentTask].TSP);//set new PSP with TSP
	return 1;
}

void SysTick_Handler(void){
	if(mutex==true){
		int i = 0;
		int timeElapsed = 0;
		msTicks++;
	
	
	
		for (i=0; i<threadCount; i++)
		{
			if(threadList[i].state == SLEEP 
				&& (msTicks-threadList[i].napStart)>=threadList[i].napLength)
			{
				threadList[i].state = WAITING; // and then remove form sleepList?
				threadList[i].napStart=0;
			}
		}
	
		//will need to determine if a change in threads has occurred 
		//and the current time passed in each/new thread
	
		//can do this with a var that stores the old index and a timer???
		
		
		if(taskSwitched)
		{
			//printf ("\n taskswitched: ");
			//printf("%d\n",taskSwitched);
			taskSwitched = 0;
			timeInThread = msTicks;
			//storeTask = osCurrentTask;
			//printf ("\n store task");
			//printf("%d\n", storeTask);
			
			//printf ("\n starttime");
			//printf("%d\n",timeInThread);
		}
		//printf("\n timeelapsed: ");
		timeElapsed = msTicks-timeInThread;
		//printf("%d\n", timeElapsed);
		
		if ((timeElapsed-FORCE_SWITCH_TIME) >= 0 && taskSwitched==0)
		{
			printf("helloooooooooooooooooooooooooooooooooo");
			pushValue = 8*4;
			osYield();
		}
		
	
	
	}
	
}

void osSleep(int time){
	threadList[osCurrentTask].state = SLEEP;
	nextState=SLEEP;
	threadList[osCurrentTask].napLength = time; //set naptime var
	threadList[osCurrentTask].napStart = msTicks; //set napt start var
	
	osYield();
}


