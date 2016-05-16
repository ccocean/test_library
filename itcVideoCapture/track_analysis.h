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

	typedef struct AnalysisTimer
	{
		ULINT start;

		ULINT end;

		double deltatime;

		struct AnalysisTimer* next;

	}Analysis_Timer_t;


	typedef struct TrackAnalysis
	{

		Analysis_Timer_t standTimer;//站立时间

		Analysis_Timer_t moveTimer;//移动时间

		int cntOutside;//下讲台次数
		Analysis_Timer_t *outTimer;//下讲台时间

		int cntMultiple;//多目标次数
		Analysis_Timer_t *mlpTimer;//多目标时间

	}Tch_Analysis_t;

	//具体函数
	
	
	Analysis_Timer_t* track_timerInit();
	Analysis_Timer_t * track_timerIncrease(Analysis_Timer_t *timer, int *count, Analysis_Timer_t *node);
	int track_timerDestroy(Analysis_Timer_t **timer, int *count);

	int track_timerStart(Analysis_Timer_t *timer);
	int track_timerEnd(Analysis_Timer_t *timer);
	int track_timerUpdate(Analysis_Timer_t *timer);


#ifdef  __cplusplus  
}
#endif  /* end of __cplusplus */ 
#endif