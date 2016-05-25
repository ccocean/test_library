#ifndef _TCH_PARAMS_H_
#define _TCH_PARAMS_H_

typedef struct Tch_Size_t
{
	int width;
	int height;
}Tch_Size_t;

typedef struct Tch_Rect_t
{
	int x;
	int y;
	int width;
	int height;
}Tch_Rect_t;

//返回结构体
typedef struct Result
{
	int status;
	int pos; //云台摄像机拍摄的位置
}Tch_Result_t;

//阈值结构体
typedef struct Threshold
{
	int stand;
	int targetArea;
	int outside;
}Tch_Threshold_t;

typedef struct 	_TeaITRACK_Params
{
	int isSetParams;
	int numOfPos;
	int numOfSlide;
	Tch_Size_t frame;
	Tch_Rect_t tch;
	Tch_Rect_t blk;
	Tch_Threshold_t threshold;

}TeaITRACK_Params;

typedef unsigned long ULINT;

typedef struct AnalysisTimerN
{
	ULINT start;

	ULINT end;

	int deltatime;

	//struct AnalysisTimerN* next;

}Analysis_Timer_node;

/*typedef struct AnalysisTimer
{
int count;
Analysis_Timer_node* head;
Analysis_Timer_node* tail;
}Analysis_Timer_t;*/

typedef struct StatisticsTimer
{
	int count;
	int deltatime;
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

#endif
