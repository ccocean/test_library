#ifndef _TRACK_ANALYSIS_
#define _TRACK_ANALYSIS_

#include "tch_params.h"

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#define  _GetTime GetTickCount
typedef struct AlgLink_Record_Status 
{
	int recordstatus;
}AlgLink_Record_Status_t;
#else
#include <mcfw/src_bios6/utils/utils.h>
#define  _GetTime Utils_getCurTimeInMsec
#endif

#define TEACHER 0
#define STUDENT 1

	

	
	//¾ßÌåº¯Êý
	
	
	//Analysis_Timer_t* track_timerCreator();
	//Analysis_Timer_t * track_timerIncrease(Analysis_Timer_t *Timer_list, Analysis_Timer_node *node);
	int track_statisticsIncrease(Statistics_Timer_t *statcs, Analysis_Timer_node *node);
	//int track_timerClear(Analysis_Timer_t *Timer_list);
	//int track_timerDestroy(Analysis_Timer_t **pTimer_list);
	//int track_timerGetCount(Analysis_Timer_t *Timer_list);
	int track_statisticGetCount(Statistics_Timer_t *statcs);

	int track_timerStart(Analysis_Timer_node *timer);
	int track_timerEnd(Analysis_Timer_node *timer);

#ifdef  __cplusplus  
}
#endif  /* end of __cplusplus */ 
#endif