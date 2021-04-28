#ifndef DSQUEUE_H
#define DSQUEUE_H

#include "sys.h"
#include "malloc.h"
#include "usart.h"
#include "config.h"


typedef struct Queue
{
	uint8_t data[DSQUEUE_SIZE][STOREDATA_LEN];
	int front;
	int rear;
}Queue, *pQueue;

Queue* init(void)
{
	Queue* q = (Queue*)mymalloc(SRAMIN, sizeof(Queue));
	
	if(q == NULL)
	{
		printf("malloc failed!\n");
		return NULL;
	}
	
	q->front = 0;
	q->rear = 0;
	
	return q;
}

int isFull(Queue* q)
{
	return ((q->rear+1)%DSQUEUE_SIZE == q->front);
}

int isEmpty(Queue* q)
{
	return (q->rear == q->front);
}

void push(Queue* q, uint8_t *pstoredata)
{
	int i = 0;
	
	if(isFull(q))
	{
		printf("Queue is full\n");
		return;
	}
	
	for(i = 0; i < 39; i++)
	{
		q->data[q->rear][i] = pstoredata[i];
	}
	
	q->rear = (q->rear+1)%DSQUEUE_SIZE;
}

uint8_t* pop(Queue* q)
{
	uint8_t * temp;
	
	if(isEmpty(q))
	{
		printf("Queue is empty\n");
		return 0;
	}
	
	temp = q->data[q->front];
	q->front = (q->front+1)%DSQUEUE_SIZE;
	
	return temp;
}

//int Queue_Test(void);

#endif
