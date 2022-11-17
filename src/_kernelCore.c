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
//bool taskSwitched = 0; //boolean to check if task has just switched (ie if one thread has switched to another)
uint32_t timeInThread = 0; //the exact moment (msticks value) when task has switched

//
int pushValue = 8*4; //default value to push if not forced to context switch by timer
int nextState = WAITING; //default state after running (if not being sent to sleep)

//running in sleep state variables/for checks
//int numSleepThreads = 0; //value to see if all threads are asleep and therefore idlethread has to run
bool allSleep = 0; //if numSleepThreads == number of threads (idle is treated similarly to sleeping for this check)

//mutex
//bool mutex;
int indexEDF = 0; //index of the earliest coming up deadline

//functions from __threadsCore.c that will be used in __kernelCore.c
extern void osCreateThread(void(*userFunction)(void *args), double freq); 
extern void osIdleTask(void* args);

//this function initializes memory structs and interrupts required to run kernel
void kernelInit(void){
	uint32_t * MSP_Original = 0; //may not need
	SHPR3 |= 0xFE << 16;	//sets priority of the PENDSV interrupt; bit shifts 0xFF by 16 and bitwise OR SHPR3 with that value
	SHPR3 |= 0xFFU << 24; //Set the priority of SysTick to be the weakest
	SHPR2 |= 0xFDU << 24; //Set the priority of SVC the be the strongest
	
	mspAddr = *MSP_Original; //get address of original MSP
	
	osCreateThread(osIdleTask,0); //idleTask is created in kernelInit so that RTOS users cannot access the idle task
	threadList[0].state = IDLE; //sets state of idle task to the special IDLE state
}

//this function is called by the kernel; it schedules which threads to run
void osYield(void){
	__ASM("SVC #0"); //trigger SVC right away
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
			//a check to ensure that a non-sleep && aperiodic thread does not try to decrement its unused napDLTimer value & overflow
			if (threadList[i].timerLength!=0) 
			{
				threadList[i].napDLTimer--; //decrement timers for all threads that are sleeping &&
			}
			//checks if a thread's state is sleep AND if its naptime is over (wakeup)
			if(threadList[i].state == SLEEP && threadList[i].napDLTimer<=0)
			{
				threadList[i].state = WAITING; //set state to WAITING so that it is ready to be called 
				//if periodic, we also want to start its deadline timer for our EDF scheduling
				if(threadList[osCurrentTask].periodic == true)  
				{
					//start waiting timer (same var name as the sleep timer in our struct)
					//set deadline timer to period length
					threadList[osCurrentTask].napDLTimer =  threadList[osCurrentTask].timerLength; 
				}
				else{
					threadList[i].napDLTimer=0; //reset napDLTimer (this is for sleeping threads)
				}
			}
		}
		
		//FORCED CONTEXT SWITCHING based on running out of time for a thread to run
		timeInThread --;
		
		//call osGetEDF and determine if there is a sooner deadline, if so, force a context switch 
		osGetEDF();
		
		//checks if the time for a task to run is over OR the index of the newest deadline is different from the current thread running 
		if(timeInThread<=0 || (osCurrentTask != indexEDF && indexEDF!= -1)) 
		{
			pushValue = 8*4; //push 8 registers bc of tail chain condition
			
			if(osCurrentTask >= 0) //checks to ensure that at least one thread exists
			{	
				//pushes (16 or 8) uint32_ts to move the TSP down below garbage registers
				threadList[osCurrentTask].TSP = (uint32_t*)(__get_PSP() - pushValue); 
				if(osCurrentTask==0) //checks if the current running task has an index of one (idleThread)
				{
					nextState = IDLE;//sets its nextState to idle
				}
				if(threadList[osCurrentTask].periodic==true) //for periodic threads, should sleep next
				{
					nextState = SLEEP;  //sets next state to sleep
					threadList[osCurrentTask].napDLTimer = threadList[osCurrentTask].timerLength; 
					//resets nap timer to given period length
			  }
				//sets the state of currentTask to its nextState for when its called again
				threadList[osCurrentTask].state = nextState; //sets state of the currenttask 
				nextState = WAITING; //defaults state back to WAITING
				pushValue = 8*4; //defaults back to 16*4 (ie 16 registers to push)
			}
	
			osSched(); //calls EDF scheduler 
			
			timeInThread = FORCE_SWITCH_TIME; //resets timer for forced context switching to constant FORCE_SWITCH_TIME
			threadList[osCurrentTask].state = ACTIVELY_RUNNING; //sets thread that will run next to ACTIVELY_RUNNING state
		
			ICSR |= 1<<28;	//changes pendSV state to pending
			__asm("isb");	//tells compiler to run the "isb" instruction using assembly
			
		}
}

int osGetEDF(void)
{
	int count; //count for going through the entire threadlist
	int soonestDeadline = 100000; //sets soonestdeadline to high number so that the actual sooner deadlines are a lower value
	indexEDF = -1; //sets starting index to an impossible value for checks in systick
	
	for(count=0; count<threadCount; count++) //goes through entire threadlist
	{
		//checks if there is a sooner deadline from a periodic thread that is not sleeping or idle
		if(threadList[count].napDLTimer < soonestDeadline && threadList[count].periodic==true 
			&& threadList[count].state!= SLEEP && threadList[count].state!= IDLE) 
		{
			indexEDF = count; //sets the index to the current thread that was just checked
			//sets the soonest deadline to the value of the timer for the same thread
			soonestDeadline = threadList[count].napDLTimer; 
		}
	}
	return indexEDF; //returns the index of the earliest deadline thread
}

void osSched(void){
	int numSleepThreads = 0; //count for the num of currently sleeping threads, gets reset to 0 whenever osSched is called
	int indexEDF = osGetEDF(); //get the current index of the earliest deadline
	
	if(indexEDF!=-1) //if it is not -1 then there is a thread with an earliest deadline
	{
		osCurrentTask=indexEDF; //return this index to be run next
	}
	else{
		//increments osCurrentTask (% is to make sure it cycles through 0 to threadCount instead of going above)
		osCurrentTask = (osCurrentTask+1)%(threadCount); 
	
		//checks to ensure we do not accidentally run the sleeping thread at all OR the idlethread without them all being asleep
		while((threadList[osCurrentTask].state==SLEEP 
			|| threadList[osCurrentTask].state == IDLE) && allSleep == 0){ 
			if(numSleepThreads==(threadCount)) //checks for condition of all threads sleeping
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
			numSleepThreads++; //increment number of sleeping threads (or it could be idle, but same idea)

		}
		allSleep = 0; //resets allSleep to false for next iteration
	}
}

//sleep function
void osSleep(int time){
	threadList[osCurrentTask].state = SLEEP; //change state of current task to SLEEP
	nextState=SLEEP; //set nextState (for when state is set in osYield) to SLEEP
	threadList[osCurrentTask].timerLength = time; //set naptime var based on given argument
	threadList[osCurrentTask].napDLTimer = time; //set the nap timer to the length given in the arguments for osSleep
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
			
			if(threadList[osCurrentTask].periodic==true) //for periodic threads, should sleep next
			{
				nextState = SLEEP; //sets next state to sleep
				// resets nap timer to given period length
				threadList[osCurrentTask].napDLTimer = threadList[osCurrentTask].timerLength; 
			}
			//sets the state of currentTask to its nextState for when its called again
			threadList[osCurrentTask].state = nextState; //sets state of the current task
			nextState = WAITING; //defaults state back to WAITING
			pushValue = 8*4; //defaults back to 16*4 (ie 16 registers to push)
		}
		
		osSched(); //calls EDF scheduler
		
		timeInThread=FORCE_SWITCH_TIME; //resets timer for forced context switching to constant FORCE_SWITCH_TIME
		threadList[osCurrentTask].state = ACTIVELY_RUNNING; //sets thread that will run next to ACTIVELY_RUNNING state
		
		ICSR |= 1<<28;	//changes pendSV state to pending
		__asm("isb");	//tells compiler to run the "isb" instruction using assembly
	}
}


