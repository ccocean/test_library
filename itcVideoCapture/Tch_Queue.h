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

/*构造一个空队列*/
Tch_Queue_t *InitQueue();

/*销毁一个队列*/
void DestroyQueue(Tch_Queue_t *pqueue);

/*清空一个队列*/
void ClearQueue(Tch_Queue_t *pqueue);

/*判断队列是否为空*/
int IsEmpty(Tch_Queue_t *pqueue);

/*返回队列大小*/
int GetSize(Tch_Queue_t *pqueue);

/*返回队头元素*/
PNode GetFront(Tch_Queue_t *pqueue, Item *pitem);

/*返回队尾元素*/
PNode GetRear(Tch_Queue_t *pqueue, Item *pitem);

/*将新元素入队*/
PNode EnQueue(Tch_Queue_t *pqueue, Item item);

/*队头元素出队*/
PNode DeQueue(Tch_Queue_t *pqueue, Item *pitem);

/*遍历队列并对各数据项调用visit函数*/
void QueueTraverse(Tch_Queue_t *pqueue);

#endif