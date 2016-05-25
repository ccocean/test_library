#include "track_analysis.h"

//Analysis_Timer_t* track_timerCreator()
//{
//	Analysis_Timer_t *timer = malloc(sizeof(Analysis_Timer_t));
//	memset(timer, 0, sizeof(Analysis_Timer_t));
//	timer->count = 0;
//	timer->head = timer->tail = NULL;
//	return timer;
//}

int track_statisticsIncrease(Statistics_Timer_t *statcs, Analysis_Timer_node *node)
{
	if (statcs == NULL || node == NULL)
	{
		return -1;
	}
	statcs->deltatime += node->deltatime;
	statcs->count++;
	return 0;
}

//Analysis_Timer_t * track_timerIncrease(Analysis_Timer_t *Timer_list, Analysis_Timer_node *node)
//{
//	if (Timer_list == NULL)
//	{
//		Timer_list = track_timerCreator();
//	}
//
//	Analysis_Timer_node *new_Node = malloc(sizeof(Analysis_Timer_node));
//	memcpy(new_Node, node, sizeof(Analysis_Timer_node));
//
//	if (Timer_list->count == 0)
//	{
//		Timer_list->count = 1;
//		Timer_list->head = Timer_list->tail = new_Node;
//		Timer_list->tail->next = NULL;
//	}
//	else
//	{
//		Timer_list->count++;
//		Timer_list->tail->next = new_Node;				//队尾插入
//		Timer_list->tail = Timer_list->tail->next;		//更新队尾
//		Timer_list->tail->next = NULL;
//	}
//	return Timer_list;
//}
//
//int track_timerClear(Analysis_Timer_t *Timer_list)
//{
//	if (Timer_list == NULL)
//	{
//		return -1;
//	}
//
//	Analysis_Timer_node *temp = NULL;
//	while (Timer_list->head != NULL)
//	{
//		temp = Timer_list->head->next;
//		free(Timer_list->head);
//		Timer_list->head = temp;
//	}
//	Timer_list->count = 0;
//
//	return 0;
//}
//
//int track_timerDestroy(Analysis_Timer_t **pTimer_list)
//{
//	if (pTimer_list == NULL ||(*pTimer_list) == NULL)
//	{
//		return -1;
//	}
//
//	track_timerClear(*pTimer_list);
//	free(*pTimer_list);
//	*pTimer_list = NULL;
//
//	return 0;
//}
//
//int track_timerGetCount(Analysis_Timer_t *Timer_list)
//{
//	if (Timer_list == NULL)
//	{
//		return -1;
//	}
//	return Timer_list->count;
//}

int track_statisticGetCount(Statistics_Timer_t *statcs)
{
	if (statcs==NULL)
	{
		return -1;
	}
	return statcs->count;
}

int track_timerStart(Analysis_Timer_node *timer)
{
	if (timer==NULL)
	{
		return -1;
	}

	timer->start = _GetTime();
	return 0;
}

int track_timerEnd(Analysis_Timer_node *timer)
{
	if (timer == NULL)
	{
		return -1;
	}
	timer->end = _GetTime();
	//timer->deltatime = 0;
	timer->deltatime += (timer->end - timer->start);
	timer->start = 0;
	return 0;
}
