#include <stdint.h>
#include <LPC17xx.h>
#include "osDefs.h"
#define QUEUE_LENGTH 8

int waitingQueue[QUEUE_LENGTH];
int front = -1;
int rear= -1;
int countQueueItems = 0;
//queue operations 
//use regular array for queueing 

//operations
/*
enqueue
dequeue
peek
*/

	
void enqueue(int waitingThreadIndex)
{
	//conditional statement that checks if the current amount of queue values are less than the total queue size
	if(countQueueItems < QUEUE_LENGTH)	{
		//conditional statement that checks if the rear value is 
		/*if(rear == QUEUE_LENGTH-1)	{
			printf("ERROR: COULD NOT ADD TO QUEUE");
			//rear = -1;
		}*/
		//else{
			if(front == -1)
				front = 0;
			
				rear = (rear+1)%8;
				waitingQueue[rear] = waitingThreadIndex;
				countQueueItems++; //increments every time a value is enqueued (added to a queue)
			
		//}
	}
	else{
		printf("ERROR: COULD NOT ADD TO QUEUE");
	}
}

void dequeue(void)
{
	if(front == -1 ||front>rear)
	{
		front = -1;
		printf("Error: underflow!");
		return;
	}
	else{
		front = (front + 1)%8;
	}
	countQueueItems--;	//decrements every time a value is dequeued (removed from queue)	
}

//returns the value at the front of the queue
int peek(void)
{
	return front;
}

void printQueue(void)
{
	int i;
	printf("[ ");
	for (i=0; i<countQueueItems; i++)
	{
		printf("%d", waitingQueue[i]);
		printf(" ");
	}
	printf("]\n");
	printf("front");
	printf("%d\n", front);
}
