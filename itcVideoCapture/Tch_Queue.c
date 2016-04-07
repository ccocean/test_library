#include "Tch_Queue.h"


/*����һ���ն���*/
Tch_Queue_t *InitQueue()
{
	Tch_Queue_t *pqueue = (Tch_Queue_t *)malloc(sizeof(Tch_Queue_t));
	if (pqueue != NULL)
	{
		pqueue->front = NULL;
		pqueue->rear = NULL;
		pqueue->size = 0;
	}
	return pqueue;
}

/*����һ������*/
void DestroyQueue(Tch_Queue_t *pqueue)
{
	if (IsEmpty(pqueue) != 1)
		ClearQueue(pqueue);
	free(pqueue);
}

/*���һ������*/
void ClearQueue(Tch_Queue_t *pqueue)
{
	while (IsEmpty(pqueue) != 1)
	{
		DeQueue(pqueue, NULL);
	}
}

/*�ж϶����Ƿ�Ϊ��*/
int IsEmpty(Tch_Queue_t *pqueue)
{
	if (pqueue->front == NULL&&pqueue->rear == NULL&&pqueue->size == 0)
		return 1;
	else
		return 0;
}

/*���ض��д�С*/
int GetSize(Tch_Queue_t *pqueue)
{
	return pqueue->size;
}

/*���ض�ͷԪ��*/
PNode GetFront(Tch_Queue_t *pqueue, Item *pitem)
{
	if (IsEmpty(pqueue) != 1 && pitem != NULL)
	{
		*pitem = pqueue->front->data;
	}
	return pqueue->front;
}

/*���ض�βԪ��*/

PNode GetRear(Tch_Queue_t *pqueue, Item *pitem)
{
	if (IsEmpty(pqueue) != 1 && pitem != NULL)
	{
		*pitem = pqueue->rear->data;
	}
	return pqueue->rear;
}

/*����Ԫ�����*/
PNode EnQueue(Tch_Queue_t *pqueue, Item item)
{
	PNode pnode = (PNode)malloc(sizeof(Node));
	if (pnode != NULL)
	{
		pnode->data = item;
		pnode->next = NULL;

		if (IsEmpty(pqueue))
		{
			pqueue->front = pnode;
		}
		else
		{
			pqueue->rear->next = pnode;
		}
		pqueue->rear = pnode;
		pqueue->size++;
	}
	return pnode;
}

/*��ͷԪ�س���*/
PNode DeQueue(Tch_Queue_t *pqueue, Item *pitem)
{
	PNode pnode = pqueue->front;
	if (IsEmpty(pqueue) != 1 && pnode != NULL)
	{
		if (pitem != NULL)
			*pitem = pnode->data;
		pqueue->size--;
		pqueue->front = pnode->next;
		free(pnode);
		if (pqueue->size == 0)
			pqueue->rear = NULL;
	}
	return pqueue->front;
}

/*�������в��Ը����������visit����*/
void QueueTraverse(Tch_Queue_t *pqueue)
{
	PNode pnode = pqueue->front;
	int i = pqueue->size;
	while (i--)
	{
		//visit(pnode->data);
		//printf(pnode->data);
		pnode = pnode->next;
	}

}