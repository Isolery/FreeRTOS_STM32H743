//#include "queue.h"

//uint8_t sdata[39] = {0};

//Queue* init(void)
//{
//	Queue* q = (Queue*)mymalloc(SRAMIN, sizeof(Queue));
//	
//	if(q == NULL)
//	{
//		printf("malloc failed!\n");
//		return NULL;
//	}
//	
//	q->front = 0;
//	q->rear = 0;
//	
//	return q;
//}

//int isFull(Queue* q)
//{
//	return ((q->rear+1)%QUEUE_SIZE == q->front);
//}

//int isEmpty(Queue* q)
//{
//	return (q->rear == q->front);
//}

//void push(Queue* q, uint8_t *pstoredata)
//{
//	int i = 0;
//	
//	if(isFull(q))
//	{
//		return;
//	}
//	
//	for(i = 0; i < 39; i++)
//	{
//		q->data[q->rear][i] = pstoredata[i];
//	}
//	
//	q->rear = (q->rear+1)%QUEUE_SIZE;
//}

//uint8_t* pop(Queue* q)
//{
//	uint8_t * temp;
//	
//	if(isEmpty(q))
//	{
//		return 0;
//	}
//	
//	temp = q->data[q->front];
//	q->front = (q->front+1)%QUEUE_SIZE;
//	
//	return temp;
//}

//int Queue_Test(void)
//{
//	Queue *q = init();
//	int i, j;
//	uint8_t *data;

//	for(j = 0; j < 9; j++)
//	{
//		for(i = 0; i < 39; i++)
//		{
//			sdata[i] = j;
//		}
//		
//		push(q, sdata);
//	}
//	
//	while(!isEmpty(q))
//	{
//		data = pop(q);
//		if(isEmpty(q))
//		{
//			for(i = 0; i < 39; i++)
//			{
//				printf("%d", data[i]);
//			}
//		}
//		else	
//		{
//			for(i = 0; i < 39; i++)
//			{
//				printf("%d", data[i]);
//			}
//		}
//	}
//	
//	return 0;
//}

