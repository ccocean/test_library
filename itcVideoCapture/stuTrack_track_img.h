#ifndef stuTrack_track_img_h__
#define stuTrack_track_img_h__

#include "itctype.h"
#include "itcerror.h"
#include "itcdatastructs.h"
#include "itcCore.h"
#include "stuTrack_settings_parameter.h"
#include "track_analysis.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define COUNT_STUTRACK_MALLOC_ELEMENT 10

#define SATUTRACK_PRINTF_LEVEL_1 1
#define SATUTRACK_PRINTF_LEVEL_2 2
#define SATUTRACK_PRINTF_LEVEL_3 3
#define SATUTRACK_PRINTF_LEVEL_4 4

#define SATUTRACK_BK_IMG_COUNT 16
#define SATUTRACK_BK_IMG_FRAME_INTERVAL 3

#define _PRINTF											\
	if (interior_params_p->callbackmsg_func == NULL)		\
	{	interior_params_p->callbackmsg_func = printf; }		\
	((callbackmsg)(interior_params_p->callbackmsg_func))

#define ITC_RETURN
#define JUDEGE_STUREACK_IF_NULL(p,r)			\
if ((p) == NULL)								\
{												\
	_PRINTF("Memory allocation error!\n");		\
	stuTrack_stopTrack(inst, interior_params_p);\
	return r;									\
}

#define ITC_FUNCNAME(name) \
	if ((interior_params_p->stuTrack_debugMsg_flag) >= SATUTRACK_PRINTF_LEVEL_4)\
	{																			\
		_PRINTF(name);															\
	}


typedef int(*_callbackmsg)(const char *format, ...);//�������������Ϣ�ĺ���ָ��

typedef struct StuTrack_Stand_t
{
	int direction;
	int count_teack;	//
	int count_up;		//
	int count_down;		//
	int flag_Stand;		//������־
	int flag_matching;	//ƥ���־
	Track_Point_t centre;
	Track_Rect_t roi;
	int origin_y;
	unsigned long start_tClock;
	unsigned long current_tClock;
}StuTrack_Stand_t;

typedef struct StuTrack_BigMoveObj_t
{
	int count_track;
	int flag_bigMove;		//��־
	int dis_threshold;		//��Ϊ���ƶ�Ŀ�����ֵ
	Track_Rect_t roi;
	unsigned long start_tClock;
	unsigned long current_tClock;
	Track_Point_t origin_position;
	Track_Point_t current_position;
}StuTrack_BigMoveObj_t;

typedef struct StuTrack_allState_t
{
	int flag_state;			//״̬��־
	int flag_matching;		//ƥ���־
	int count_teack;		//
	int count_up;			//
	int count_down;			//
	int dis_threshold;		//��Ϊ���ƶ�Ŀ�����ֵ
	Track_Rect_t roi;
	unsigned long start_tClock;			//��⵽Ŀ���ʱ��
	unsigned long current_tClock;		//��ǰƥ���ʱ���
	unsigned long current_wholeClock;	//��ǰ����ƥ���ʱ���
	unsigned long upstand_tClock;		//�ж�Ϊ������ʱ���
	unsigned long moveBig_tClock;		//�ж��ƶ�Ŀ���ʱ���
	int origin_top_y;	
	double topy_diffThreshold;
	Track_Point_t origin_position;
	Track_Point_t current_position;
	Track_Point_t standUp_position;
	int standUp_size;
}StuTrack_allState_t;

typedef struct _StuITRACK_InteriorParams
{
	itc_BOOL initialize_flag;
	unsigned int _count;		//ͳ��֡��
	Track_Size_t img_size;		//����ͼ���С
	Track_Size_t srcimg_size;	//ԭʼͼ���С

	int result_flag;					//��ǰ֡�仯״̬

	int count_stuTrack_rect;			//�˶��������
	Track_Rect_t *stuTrack_rect_arr;

	int count_trackObj_allState;		//���ٵ�Ŀ����
	StuTrack_allState_t* stuTrack_allState;

	int stuTrack_debugMsg_flag;					//������Ϣ����ȼ�
	int stuTrack_Draw_flag;						//�Ƿ���ƽ��
	int stuTrack_direct_range;					//����ʱ����ĽǶ�ƫ�뷶Χ
	int stuTrack_standCount_threshold;			//�ж�Ϊ������֡����ֵ
	int stuTrack_sitdownCount_threshold;		//�ж�Ϊ���µ�֡����ֵ
	int stuTrack_moveDelayed_threshold;			//�ƶ�Ŀ�걣�ָ��ٵ���ʱ���������ʱ�����˶������������(��λ������)
	int stuTrack_deleteTime_threshold;			//Ŀ�걣�ָ��ٵ��ʱ��ʱ���������ʱ��ɾ��Ŀ��(��λ������)
	double stuTrack_move_threshold;				//�ж����ƶ�Ŀ���ƫ����ֵ����ֵ��
	double stuTrack_standup_threshold;			//�ж�������Ŀ���ƫ����ֵ����ֵ��
	int *stuTrack_size_threshold;				//�˶�Ŀ���С������ֵ������λ�ò�ͬ��ֵ��ͬ��
	int *stuTrack_direct_threshold;				//�����ı�׼�Ƕ�,��СΪwidth

	int rangeStart;
	int rangeEnd;
	int rangeStartX;
	int rangeEndX;

	Itc_Mat_t *transformationMatrix;				//ͼ����������̨����ı任����
	double stretchingAB[2];							//����ϵ��

	unsigned int last_upcast_time;					//��¼��һ���׳�״̬��Ϣ��ʱ��
	unsigned int move_camera_time;					//��һ���ƶ������ʱ��
	unsigned int moveStopObj_time;					//Ŀ��ֹͣ�ƶ�ʱ��
	int old_move_Stopflag;							//�Ƿ����ƶ�Ŀ��ֹͣ�������
	int OldResult_flag;								//��һ�εķ��ؽ��
	int move_csucceed_flag;							//Ҫ�ƶ���ͷʱ���øñ��Ϊfalse���ɹ������ƶ��źź�����true

	TrackPrarms_Point_t old_move_position;			//�ƶ�Ŀ�걣��ľ�ͷλ��
	int old_move_stretchingCoefficient;				//�ƶ�Ŀ�걣�������ϵ��
	TrackPrarms_Point_t old_standup_position;		//��������ľ�ͷλ��
	int old_standup_stretchingCoefficient;			//����Ŀ�걣�������ϵ��

	Itc_Mat_t *tempMat;
	Itc_Mat_t *currMat;
	Itc_Mat_t *lastMat;
	Itc_Mat_t *mhiMat;
	Itc_Mat_t *maskMat;

	int current_BKid;
	Itc_Mat_t *BKmat[SATUTRACK_BK_IMG_COUNT];
	Track_MemStorage_t* stuTrack_storage;

	//���ڻ��Ƶ���ɫ
	Track_Colour_t pink_colour;					/*�ۺ�*/
	Track_Colour_t blue_colour;					/*����*/
	Track_Colour_t lilac_colour;				/*����*/
	Track_Colour_t green_colour;				/*����*/
	Track_Colour_t red_colour;					/*����*/
	Track_Colour_t dullred_colour;				/*����*/
	Track_Colour_t yellow_colour;				/*����*/

	int bStat_flag;
	Analysis_Timer_node standUpStatTemp;
	Stu_Analysis_t  stu_statistics;

	_callbackmsg callbackmsg_func;					//������Ϣ����ĺ���ָ��
}StuITRACK_InteriorParams;

typedef struct _StuITRACK_SystemParams
{
	int nsrcHeight;			//Դͼ��߶�
	int nsrcWidth;			//Դͼ����
	_callbackmsg callbackmsg_func;					//������Ϣ����ĺ���ָ��
}StuITRACK_SystemParams_t;

typedef struct 	_StuITRACK_Params
{
	StuITRACK_SystemParams_t systemParams;
	StuITRACK_ClientParams_t clientParams;
}StuITRACK_Params;

//�仯״̬��
#define RESULT_STUTRACK_NULL_FLAG		0
#define	RESULT_STUTRACK_STANDUP_FLAG	1
#define	RESULT_STUTRACK_SITDOWN_FLAG	2
#define	RESULT_STUTRACK_MOVE_FLAG		4
#define RESULT_STUTRACK_STOPMOVE_FLAG	8

//�⼸�����result_flag�����ж�
#define RESULT_STUTRACK_IF_STANDUP(n)		((n & RESULT_STUTRACK_STANDUP_FLAG)== RESULT_STUTRACK_STANDUP_FLAG)		//�ж��Ƿ��������Ķ���
#define RESULT_STUTRACK_IF_SITDOWN(n)		((n & RESULT_STUTRACK_SITDOWN_FLAG)== RESULT_STUTRACK_SITDOWN_FLAG)		//�ж��Ƿ������µĶ���
#define RESULT_STUTRACK_IF_MOVE(n)			((n & RESULT_STUTRACK_MOVE_FLAG)== RESULT_STUTRACK_MOVE_FLAG)			//�ж��Ƿ����µ��ƶ�Ŀ��
#define RESULT_STUTRACK_IF_STOPMOVE(n)		((n & RESULT_STUTRACK_STOPMOVE_FLAG)== RESULT_STUTRACK_STOPMOVE_FLAG)	//�ж��Ƿ����ƶ�Ŀ��ֹͣ�˶�

//Ĭ���������ֵ
#define SCALE_STURACK_DEFAULT_ZOOM						0.333333333333333333
#define THRESHOLD_STUTRACK_MOVE_DEFALUT_PARAMS			1.0				//�ƶ���ֵ
#define THRESHOLD_STUTRACK_STANDUP_DEFALUT_PARAMS		0.6				//����ƫ����ֵ
#define THRESHOLD_STUTRACK_STANDCOUNT_DEFALUT_PARAMS	8				//��������
#define THRESHOLD_STUTRACK_SITDOWNCOUNT_DEFALUT_PARAMS	8				//���¼���
#define THRESHOLD_STUTRACK_MOVEDELAYED_DEFALUT_TIME		2000		//1��
#define THRESHOLD_STURECK_ALL_DELETE_TIME				65000		//65��
#define RANGE_STUTRACK_STANDDIRECT_DEFALUT_PARAMS		25

//Ĭ�ϵı任����
#define MATRIX_STUTRACK_DEFAULT_PARAMS double m[9]=\
	{2.7, 0.0,-600.0,\
	 0.0, -2.8, 380.0,\
	 0.0, 0.0,   1.0};

#define MATRIX_STUTRACK_DEFAULT_PARAMS_AT(n) m[n]

#define STRETCHING_STUTRACK_DEFFAULT_PARAMS_A (-90)
#define STRETCHING_STUTRACK_DEFFAULT_PARAMS_B (15000)
//���ڼ���ɸѡ��ֵ�����Է��̲���
#define A_STUTRACK_SIZE_THRESHOLD_PARAMS		(0.0005)
#define B_STUTRACK_SIZE_THRESHOLD_PARAMS		(13.0)
#define DIRECT_STUTRACK_TRANSMUTABLILITY_RANGE	(0.0)
#define A_STUTRACK_DIRECT_THRESHOLD_PARAMS		(DIRECT_STUTRACK_TRANSMUTABLILITY_RANGE/WIDTH_STUTRACK_IMG_)
#define B_STUTRACK_DIRECT_THRESHOLD_PARAMS		(270-DIRECT_STUTRACK_TRANSMUTABLILITY_RANGE/2)
#define MINTHRESHOLD_STUTRACK_SIZE_THRESHOLD_PARAMS		13
#define MAXTHRESHOLD_STUTRACK_SIZE_THRESHOLD_PARAMS		55
#define MINTHRESHOLD_STUTRACK_DIRECT_THRESHOLD_PARAMS	225
#define MAXTHRESHOLD_STUTRACK_DIRECT_THRESHOLD_PARAMS	315

#define COMPUTER_STUTRACK_SIZE_THRESHOLD_PARAMS(n,a,b)  (ITC_MIN(ITC_MAX(((a *n*n + b)), MINTHRESHOLD_STUTRACK_SIZE_THRESHOLD_PARAMS), MAXTHRESHOLD_STUTRACK_SIZE_THRESHOLD_PARAMS))
#define COMPUTER_STUTRACK_DIRECT_THRESHOLD_PARAMS(n,a,b)  (ITC_MIN(ITC_MAX(((a *n + b)), MINTHRESHOLD_STUTRACK_DIRECT_THRESHOLD_PARAMS), MAXTHRESHOLD_STUTRACK_DIRECT_THRESHOLD_PARAMS))


itc_BOOL stuTrack_initializeTrack(StuITRACK_Params *inst, StuITRACK_InteriorParams* interior_params_p);

typedef int stuTrackReturn;
#define RETURN_STUTRACK_NEED_PROCESS 1		//��Ҫ����
#define RETURN_STUTRACK_noNEED_PROCESS 0	//����Ҫ����
//************************************
// ��������: stuTrack_Process
// ����˵������������ÿһ֡��ͼ�����ݣ���ѧ����������������ٺ��ƶ�����
// ��    �ߣ�XueYB
// �������ڣ�2015/12/12
// �� �� ֵ: RETURN_STUTRACK_NEED_PROCESS��ʾreturn_params���б仯�ģ�RETURN_STUTRACK_noNEED_PROCESS��ʾû�б仯����Ҫ����
// ��    ��: inst�������ϵͳ������interior_params_p�ñ�����ٱ����Ͳ�����return_params������ĸ��ٽ��
//************************************
stuTrackReturn stuTrack_process(const StuITRACK_Params *inst, StuITRACK_InteriorParams* interior_params_p, StuITRACK_OutParams_t* return_params, char* imageData, char* bufferuv);

void stuTrack_stopTrack(const StuITRACK_Params *inst, StuITRACK_InteriorParams* interior_params_p);

void stuTrack_statisticsSwitch(StuITRACK_InteriorParams* interior_params_p,int flag);

Stu_Analysis_t* stuTrack_statisticsGet(StuITRACK_InteriorParams* interior_params_p);

#ifdef  __cplusplus  
}
#endif  /* end of __cplusplus */ 

#endif
