#ifndef _TRACK_ANALYSIS_
#define _TRACK_ANALYSIS_

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#define  _GetTime GetTickCount
#else
#include <mcfw/src_bios6/utils/utils.h>
#define  _GetTime Utils_getCurTimeInMsec
#endif

#define TEACHER 0
#define STUDENT 1

	typedef unsigned long ULINT;

	typedef struct AnalysisTimerN
	{
		ULINT start;

		ULINT end;

		double deltatime;

		struct AnalysisTimerN* next;

	}Analysis_Timer_node;

	typedef struct AnalysisTimer
	{
		int count;
		Analysis_Timer_node* head;
		Analysis_Timer_node* tail;
	}Analysis_Timer_t;

	typedef struct StatisticsTimer
	{
		int count;
		Analysis_Timer_node timer;
	}Statistics_Timer_t;

	typedef struct TrackAnalysis
	{
		Analysis_Timer_node deration;

		Analysis_Timer_node standTimer;//站立时间

		Analysis_Timer_node moveTimer;//移动时间

		//int cntOutside;//下讲台次数
		Statistics_Timer_t outTimer;//下讲台时间

		//int cntMultiple;//多目标次数
		Statistics_Timer_t mlpTimer;//多目标时间

	}Tch_Analysis_t;

	typedef struct TrackAnalysis1
	{
		Analysis_Timer_t standUpTimer;
		int moveCount;
	}Stu_Analysis_t;
	//具体函数
	
	
	Analysis_Timer_t* track_timerCreator();
	Analysis_Timer_t * track_timerIncrease(Analysis_Timer_t *Timer_list, Analysis_Timer_node *node);
	int track_statisticsIncrease(Statistics_Timer_t *statcs, Analysis_Timer_node *node);
	int track_timerClear(Analysis_Timer_t *Timer_list);
	int track_timerDestroy(Analysis_Timer_t **pTimer_list);
	int track_timerGetCount(Analysis_Timer_t *Timer_list);
	int track_statisticGetCount(Statistics_Timer_t *statcs);

	int track_timerStart(Analysis_Timer_node *timer);
	int track_timerEnd(Analysis_Timer_node *timer);

#ifdef  __cplusplus  
}
#endif  /* end of __cplusplus */ 
#endif