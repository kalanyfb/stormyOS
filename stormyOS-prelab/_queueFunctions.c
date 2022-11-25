#include <stdint.h>
#include <LPC17xx.h>

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

void enqueue(int index);
void dequeue();
int peek();
	
void enqueue(int index)
{
	//conditional statement that checks if the current amount of queue values are less than the total queue size
	if(countQueueItems < QUEUE_LENGTH)	{
		//conditional statement that checks if the rear value is 
		if(rear == QUEUE_LENGTH-1)	{
			rear = -1;
		}
		
		waitingQueue[++rear] = index;	//
		countQueueItems++;	//increments every time a value is enqueued (added to a queue)
	}
}

void dequeue()
{
	
	countQueueItems--;	//decrements every time a value is dequeued (removed from queue)
	
	
	
}

//returns the value at the front of the queue
int peek()
{
	return front;
}
