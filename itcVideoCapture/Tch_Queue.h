#ifndef Queue_H  
#define Queue_H  

#ifndef WIN32

    #include<stdlib.h>
#else
    #include<malloc.h>  
    #include<stdio.h>  
#endif


typedef int Item;
typedef struct node * PNode;
typedef struct node
{
	Item data;
	PNode next;
}Node;

typedef struct
{
	PNode front;
	PNode rear;
	int size;
}Tch_Queue_t;

/*����һ���ն���*/
Tch_Queue_t *InitQueue();

/*����һ������*/
void DestroyQueue(Tch_Queue_t *pqueue);

/*���һ������*/
void ClearQueue(Tch_Queue_t *pqueue);

/*�ж϶����Ƿ�Ϊ��*/
int IsEmpty(Tch_Queue_t *pqueue);

/*���ض��д�С*/
int GetSize(Tch_Queue_t *pqueue);

/*���ض�ͷԪ��*/
PNode GetFront(Tch_Queue_t *pqueue, Item *pitem);

/*���ض�βԪ��*/
PNode GetRear(Tch_Queue_t *pqueue, Item *pitem);

/*����Ԫ�����*/
PNode EnQueue(Tch_Queue_t *pqueue, Item item);

/*��ͷԪ�س���*/
PNode DeQueue(Tch_Queue_t *pqueue, Item *pitem);

/*�������в��Ը����������visit����*/
void QueueTraverse(Tch_Queue_t *pqueue);

#endif