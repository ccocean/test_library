#include "track_analysis.h"

Analysis_Timer_t* track_timerInit()
{
	Analysis_Timer_t *timer = malloc(sizeof(Analysis_Timer_t));
	memset(timer, 0, sizeof(Analysis_Timer_t));
	timer->next = NULL;
	return timer;
}

Analysis_Timer_t * track_timerIncrease(Analysis_Timer_t *head, int *count, Analysis_Timer_t *node)
{
	Analysis_Timer_t *temp = track_timerInit();
	memcpy(temp, node, sizeof(Analysis_Timer_t));
	if (head == NULL)
	{
		head = temp;
		head->next = NULL;
		(*count)++;
		return head;
	}

	int num=0;
	
	Analysis_Timer_t *ptr = head;
	while (1)
	{
		if (num==((*count)-1))
		{
			ptr->next = temp;
			temp->next = NULL;
			(*count)++;
			break;
		}
		else
		{
			ptr = ptr->next;
			num++;
		}
	}
	return head;
}

int track_timerStart(Analysis_Timer_t *timer)
{
	if (timer==NULL)
	{
		return -1;
	}

	timer->start = _GetTime();
	return 0;
}

int track_timerEnd(Analysis_Timer_t *timer)
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

int track_timerUpdate(Analysis_Timer_t *timer)
{
	if (timer==NULL)
	{
		return -1;
	}
	timer->end = _GetTime();
	timer->deltatime += (timer->end - timer->start);
	timer->start = timer->end;
	return 0;
}


int track_timerDestroy(Analysis_Timer_t **head, int *count)
{
	if ((*head)==NULL)
	{
		return -1;
	}
	Analysis_Timer_t *p = (*head);
	while ((*head)!=NULL)
	{
		*head = (*head)->next;
		free(p);
		p = *head;
	}
	*head = NULL;
	*count = 0;
	return 0;
}