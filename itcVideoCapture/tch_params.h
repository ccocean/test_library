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

//���ؽṹ��
typedef struct Result
{
	int status;
	int pos; //��̨����������λ��
}Tch_Result_t;

//��ֵ�ṹ��
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

	Analysis_Timer_node standTimer;//վ��ʱ��

	Analysis_Timer_node moveTimer;//�ƶ�ʱ��

	//int cntOutside;//�½�̨����
	Statistics_Timer_t outTimer;//�½�̨ʱ��

	//int cntMultiple;//��Ŀ�����
	Statistics_Timer_t mlpTimer;//��Ŀ��ʱ��

}Tch_Analysis_t;

#endif
