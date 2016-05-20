#include "stuTrack_track_img.h"
#include "itcTrack_draw_img.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define STATE_STUTRACK_NULL_FLAG	0
#define STATE_STUTRACK_STANDUP_FLAG	1
#define STATE_STUTRACK_SITDOWN_FLAG	2
#define STATE_STUTRACK_MOVE_FLAG	4
#define STATE_STUTRACK_BIG_FLAG		8

#define EXPAND_STUTRACK_INTERSECT_RECT (1)
static void stuTrack_filtrate_contours(StuITRACK_InteriorParams* interior_params_p, Track_Contour_t** pContour)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_filtrate_contours\n");
	//����ɸѡ
	if (*pContour == NULL || interior_params_p == NULL)
	{	
		return;
	}

	Track_Rect_t *stuTrack_rect_arr = interior_params_p->stuTrack_rect_arr;
	Track_Contour_t	*Contour = *pContour;
	int count_rect = 0;
	int *stuTrack_size_threshold = interior_params_p->stuTrack_size_threshold;
	do
	{
		Track_Rect_t rect = Contour->rect;
		int centre_y = rect.y + (rect.height>>1);
		if (rect.width > stuTrack_size_threshold[centre_y] &&
			rect.height > (stuTrack_size_threshold[centre_y] >> 1) &&
			count_rect < COUNT_STUTRACK_MALLOC_ELEMENT)					//ɸѡ
		{
			*(stuTrack_rect_arr + count_rect) = rect;
			count_rect++;
		}
		Contour = (Track_Contour_t*)Contour->h_next;
	} while (Contour != *pContour);

	interior_params_p->count_stuTrack_rect = count_rect;
}

#define EXPADN_STURECK_SITDOWN_DIRECT	8
#define EXPADN_STURECK_STANDUP_DIRECT	10
static int stuTrack_matchingSatnd_ROI(StuITRACK_InteriorParams* interior_params_p, Track_Rect_t roi)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_matchingSatnd_ROI\n");
	int x = roi.x + (roi.width >> 1);
	int y = roi.y + (roi.height >> 1);
	StuTrack_allState_t* stuTrack_allState = interior_params_p->stuTrack_allState;
	int count_trackObj_allState = interior_params_p->count_trackObj_allState;
	int stuTrack_direct_range = interior_params_p->stuTrack_direct_range;
	int *stuTrack_direct_threshold = interior_params_p->stuTrack_direct_threshold;
	int *stuTrack_size_threshold = interior_params_p->stuTrack_size_threshold;
	double stuTrack_move_threshold = interior_params_p->stuTrack_move_threshold;//�����ж��ƶ�Ŀ�����ֵ
	int standard_direct = stuTrack_direct_threshold[roi.x + (roi.width >> 1)];
	int size_threshold = stuTrack_size_threshold[roi.y + (roi.height >> 1)];
	int standard_direct_Dir = (standard_direct > ITC_180DEGREE) ? (standard_direct - ITC_180DEGREE) : (standard_direct + ITC_180DEGREE);

	int direct = 0;
	float dX = 0.0, dY = 0.0;
	if (interior_params_p->count_trackObj_allState>0)
	{
		//����ƥ�����,��Ҫ���������˶��;ֲ��˶���
		//int mindis = INT_MAX;
		int min_ID = -1;
		int i = 0;
		Track_Rect_t _roi;
		for (i = 0; i < count_trackObj_allState; i++)
		{
			_roi = roi;
			if (track_intersect_rect(&_roi, &stuTrack_allState[i].roi, -((stuTrack_allState[i].roi.width*stuTrack_allState[i].roi.height) >> 1)))
			{
				//int dis = (stuTrack_allState[i].current_position.x - x)*(stuTrack_allState[i].current_position.x - x) + (stuTrack_allState[i].current_position.y - y)*(stuTrack_allState[i].current_position.y - y);
				//mindis = dis;

				min_ID = i;
				int maxWidth = ITC_IMAX(stuTrack_allState[min_ID].roi.width, roi.width) >> 1;
				int maxHeight = ITC_IMAX(stuTrack_allState[min_ID].roi.height, roi.height) >> 1;
				stuTrack_allState[min_ID].count_teack++;
				stuTrack_allState[min_ID].current_tClock = gettime();
				stuTrack_allState[min_ID].flag_matching = TRUE;		//����ƥ��ɹ����
				int maxWidth2 = maxWidth >> 1;
				_roi = roi;
				_roi.x += (_roi.width >> 4);
				_roi.width -= (_roi.width >> 3);
				_roi.height = ITC_IMIN(roi.width, roi.height);
				int flag_ROI = track_calculateDirect_ROI(interior_params_p->mhiMat, _roi, &direct, &dX, &dY);	//�����˶�����
				if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_3)
				{
					_PRINTF("matchingID:%d,flag_ROI:%d,position:%d,%d,angle��%d,standard_direct��%d,new size:%d,%d,old size:%d,%d\n", min_ID, flag_ROI, stuTrack_allState[min_ID].current_position.x, stuTrack_allState[min_ID].current_position.y, direct, standard_direct, roi.width, roi.height, stuTrack_allState[min_ID].roi.width, stuTrack_allState[min_ID].roi.height);
				}
				if ((stuTrack_allState[min_ID].roi.width - roi.width) < maxWidth2)
				{
					//�����˶�
					stuTrack_allState[min_ID].current_wholeClock = gettime();
					if (stuTrack_allState[min_ID].flag_state != STATE_STUTRACK_STANDUP_FLAG)
					{
						stuTrack_allState[min_ID].current_position = itcPoint(x, y);
						stuTrack_allState[min_ID].roi = roi;//��������λ��
						if (stuTrack_allState[min_ID].roi.y > stuTrack_allState[min_ID].origin_top_y)
						{
							stuTrack_allState[min_ID].origin_top_y = stuTrack_allState[min_ID].roi.y;
						}
						if (abs(stuTrack_allState[min_ID].roi.y - roi.y) > (stuTrack_allState[min_ID].roi.width >> 1))//����ͻ��
						{
							stuTrack_allState[min_ID].count_up = 0;//Ϊ�˼�������
						}

						if ((abs(standard_direct - direct) <= stuTrack_direct_range) && flag_ROI == 1)
						{
							stuTrack_allState[min_ID].count_up++;
						}
						else
						{
							stuTrack_allState[min_ID].count_up = stuTrack_allState[min_ID].count_up > 0 ? stuTrack_allState[min_ID].count_up - 1 : 0;
						}
					}
					else
					{
						if (stuTrack_allState[min_ID].roi.width < roi.width)
						{
							int centre_x = roi.x + (roi.width >> 1);
							stuTrack_allState[min_ID].roi.x = centre_x - (stuTrack_allState[min_ID].roi.width >> 1);
							stuTrack_allState[min_ID].roi.width = ITC_IMIN((interior_params_p->img_size.width - roi.x - 1), stuTrack_allState[min_ID].roi.width);//��ҪԽ��
						}
						if (abs(stuTrack_allState[min_ID].roi.y - roi.y) < (stuTrack_allState[min_ID].roi.width >> 2))//��Ӧ�ô���ͻ��
						{
							stuTrack_allState[min_ID].roi.y = roi.y;
							if (stuTrack_allState[min_ID].roi.y < stuTrack_allState[min_ID].origin_top_y)
							{
								stuTrack_allState[min_ID].origin_top_y = stuTrack_allState[min_ID].roi.y;
							}
							stuTrack_allState[min_ID].roi.height = ITC_IMIN(interior_params_p->img_size.height - roi.y - 1, stuTrack_allState[min_ID].roi.height);//��ҪԽ��
							stuTrack_allState[min_ID].current_position = itcPoint(stuTrack_allState[min_ID].roi.x + (stuTrack_allState[min_ID].roi.width >> 1), stuTrack_allState[min_ID].roi.y + (stuTrack_allState[min_ID].roi.height >> 1));

							if ((abs(standard_direct_Dir - direct) <= (stuTrack_direct_range + EXPADN_STURECK_SITDOWN_DIRECT))
								&& flag_ROI == 1)
							{
								stuTrack_allState[min_ID].count_down++;
							}
							else
							{
								stuTrack_allState[min_ID].count_down = stuTrack_allState[min_ID].count_down > 0 ? stuTrack_allState[min_ID].count_down - 1 : 0;
							}
						}
					}
				}
				else
				{
					if (stuTrack_allState[min_ID].flag_state == STATE_STUTRACK_STANDUP_FLAG)
					{
						if (abs(stuTrack_allState[min_ID].roi.y - roi.y) < (stuTrack_allState[min_ID].roi.width >> 2))//��Ӧ�ô���ͻ��
						{
							if (stuTrack_allState[min_ID].roi.width < roi.width)
							{
								int centre_x = roi.x + (roi.width >> 1);
								stuTrack_allState[min_ID].roi.x = centre_x - (stuTrack_allState[min_ID].roi.width >> 1);
								stuTrack_allState[min_ID].roi.width = ITC_IMIN((interior_params_p->img_size.width - roi.x - 1), stuTrack_allState[min_ID].roi.width);//��ҪԽ��
							}
							stuTrack_allState[min_ID].roi.y = roi.y;
							if (stuTrack_allState[min_ID].roi.y<stuTrack_allState[min_ID].origin_top_y)
							{
								stuTrack_allState[min_ID].origin_top_y = stuTrack_allState[min_ID].roi.y;
							}
							stuTrack_allState[min_ID].roi.height = ITC_IMIN(interior_params_p->img_size.height - roi.y - 1, stuTrack_allState[min_ID].roi.height);//��ҪԽ��
							stuTrack_allState[min_ID].current_position = itcPoint(stuTrack_allState[min_ID].roi.x + (stuTrack_allState[min_ID].roi.width >> 1), stuTrack_allState[min_ID].roi.y + (stuTrack_allState[min_ID].roi.height >> 1));

							if (((stuTrack_allState[min_ID].roi.width - roi.width) < maxWidth)
								&& ((stuTrack_allState[min_ID].roi.height - roi.height) < maxHeight)
								)
							{
								if ((abs(standard_direct_Dir - direct) <= (stuTrack_direct_range + EXPADN_STURECK_SITDOWN_DIRECT))
									&& flag_ROI == 1)
								{
									stuTrack_allState[min_ID].count_down++;
								}
								else
								{
									stuTrack_allState[min_ID].count_down = stuTrack_allState[min_ID].count_down > 0 ? stuTrack_allState[min_ID].count_down - 1 : 0;
								}
							}
						}
					}
				}
			}
		}

		if (min_ID >= 0)
		{
			return 1;//ƥ��ɹ�ֱ�ӷ���
		}		//	end if(min_ID >= 0)
		else if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_3)
		{
			_PRINTF(">>>>>>>>>>>>no matching!roi size:%d,%d\n", roi.width, roi.height);
		}
	}
	
	if (interior_params_p->count_trackObj_allState < COUNT_STUTRACK_MALLOC_ELEMENT)
	{
		//add
		if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_3)
		{
			_PRINTF(">>>>>>>>>>>>add trackObj��ID:%d,origin:%d,%d,size:%d,%d\n", count_trackObj_allState, x, y, roi.width, roi.height);
		}
		stuTrack_allState[count_trackObj_allState].count_teack = 1;
		stuTrack_allState[count_trackObj_allState].count_up = 0;
		stuTrack_allState[count_trackObj_allState].count_down = 0;
		stuTrack_allState[count_trackObj_allState].flag_state = STATE_STUTRACK_NULL_FLAG;
		stuTrack_allState[count_trackObj_allState].flag_matching = TRUE;
		stuTrack_allState[count_trackObj_allState].dis_threshold = (int)(ITC_IMIN(roi.width, size_threshold)*stuTrack_move_threshold);
		stuTrack_allState[count_trackObj_allState].roi = roi;
		stuTrack_allState[count_trackObj_allState].start_tClock = stuTrack_allState[count_trackObj_allState].current_tClock = stuTrack_allState[count_trackObj_allState].current_wholeClock = gettime();
		stuTrack_allState[count_trackObj_allState].upstand_tClock = 0;
		stuTrack_allState[count_trackObj_allState].moveBig_tClock = 0;
		stuTrack_allState[count_trackObj_allState].origin_top_y = roi.y;
		stuTrack_allState[count_trackObj_allState].topy_diffThreshold = size_threshold;
		stuTrack_allState[count_trackObj_allState].origin_position = stuTrack_allState[count_trackObj_allState].current_position = itcPoint(x, y);
		interior_params_p->count_trackObj_allState++;
	}
	return 1;
}

#define THRESHOLD_STURECK_MOVETIME_DELETE_TIME	300
#define THRESHOLD_STURECK_UPtoMOVE_TIME		3000
#define THRESHOLD_STURECK_HISTOGRAM_UP_Dvalue	0.4
#define THRESHOLD_STURECK_HISTOGRAM_DOWN_Dvalue	0.4
#define THRESHOLD_STURECK_HISTOGRAM_Dvalue		0.55
#define OLDBK_STURECK_IMG_FREAM_ID	8
#define EXPADN_STURECK_STANDUP_HEIGHT_PARAMS	1.3
#define THRESHOLD_STURECK_MOVE_STOP_TIME		400				//��ΪĿ����ͣ������ʱ����
static void stuTrack_analyze_ROI(StuITRACK_InteriorParams* interior_params_p)
{
	StuTrack_allState_t* stuTrack_allState = interior_params_p->stuTrack_allState;

	double stuTrack_standTomove_threshold = ITC_MAX(interior_params_p->stuTrack_move_threshold,0.9);//�����ж�����Ŀ����û�б��ƶ�Ŀ�����ֵ
	double stuTrack_standup_threshold = interior_params_p->stuTrack_standup_threshold;	//�����ж��������ȴ�С
	int *stuTrack_size_threshold = interior_params_p->stuTrack_size_threshold;
	int stuTrack_standCount_threshold = interior_params_p->stuTrack_standCount_threshold;
	int stuTrack_sitdownCount_threshold = interior_params_p->stuTrack_sitdownCount_threshold;
	unsigned int stuTrack_moveDelayed_threshold = (unsigned int)interior_params_p->stuTrack_moveDelayed_threshold;
	unsigned int stuTrack_deleteTime_threshold = (unsigned int)interior_params_p->stuTrack_deleteTime_threshold;
	int stuTrack_direct_range = interior_params_p->stuTrack_direct_range;
	int *stuTrack_direct_threshold = interior_params_p->stuTrack_direct_threshold;

	unsigned int _time = 0;
	unsigned int _time2 = 0;
	int direct = 0;
	float dX = 0.0, dY = 0.0;
	//�ϲ��ص�����
	int i = 0;
	int j = 0;
	for (i = 0; i < interior_params_p->count_trackObj_allState; i++)
	{
		for (j = 0; j < interior_params_p->count_trackObj_allState; j++)
		{
			if (i != j)
			{
				Track_Rect_t _roi = stuTrack_allState[i].roi;
				if (track_intersect_rect(&_roi, &(stuTrack_allState[j].roi), -((_roi.width*_roi.height) >> 2)))
				{
					//����Ŀ�걻��Ŀ�긲�ǣ����������Ŀ��
					if (stuTrack_allState[i].flag_state == stuTrack_allState[j].flag_state
						|| (stuTrack_allState[i].flag_state == STATE_STUTRACK_BIG_FLAG && stuTrack_allState[j].flag_state == STATE_STUTRACK_MOVE_FLAG)//�ƶ�Ŀ��ʹ�Ŀ�굱��ͬһ��
						|| (stuTrack_allState[i].flag_state == STATE_STUTRACK_MOVE_FLAG && stuTrack_allState[j].flag_state == STATE_STUTRACK_BIG_FLAG))
					{
						//ɾ��
						int k = 0;
						(interior_params_p->count_trackObj_allState)--;
						for (k = j; k < interior_params_p->count_trackObj_allState; k++)
						{
							stuTrack_allState[k] = stuTrack_allState[k + 1];
						}
						j--;
						if (i > j)
						{
							i--;
						}
					}
					else
					{
						if ((stuTrack_allState[i].flag_state == STATE_STUTRACK_STANDUP_FLAG || stuTrack_allState[i].flag_state == STATE_STUTRACK_SITDOWN_FLAG)
							&& stuTrack_allState[j].flag_state != STATE_STUTRACK_NULL_FLAG)
						{
							stuTrack_allState[i].flag_state = stuTrack_allState[j].flag_state;
							if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
							{
								_PRINTF("!!!!!!!!!!!!!!!!!!!stand merge bigMove��stand roi:x%d,y%d,w%d,h%d,bigMove roi:x%d,y%d,w%d,h%d\n", stuTrack_allState[i].roi.x, stuTrack_allState[i].roi.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height, stuTrack_allState[j].roi.x, stuTrack_allState[j].roi.y, stuTrack_allState[j].roi.width, stuTrack_allState[j].roi.height);
							}
						}
					}
				}
			}
		}
	}
	
	for (i = 0; i < interior_params_p->count_trackObj_allState; i++)
	{
		//�ж��Ƿ���Ҫɾ����ѡ��
		_time = gettime() - stuTrack_allState[i].current_tClock;		//��⵽�˶���ʱ����
		_time2 = gettime() - stuTrack_allState[i].current_wholeClock;	//��⵽�����˶���ʱ����
		if (_time > stuTrack_moveDelayed_threshold
			|| _time2 > 2 * stuTrack_moveDelayed_threshold)
		{
			if (stuTrack_allState[i].flag_state != STATE_STUTRACK_STANDUP_FLAG	//��վ����Ŀ��ֱ��ɾ��
				|| _time2 > stuTrack_deleteTime_threshold)							//վ��Ŀ�곬�����ʱ��(1����)Ҳɾ��
			{
				if (stuTrack_allState[i].flag_state == RESULT_STUTRACK_MOVE_FLAG
					|| stuTrack_allState[i].flag_state == STATE_STUTRACK_BIG_FLAG)
				{
					interior_params_p->result_flag |= RESULT_STUTRACK_STOPMOVE_FLAG;	//���֮ǰ���˶���Ŀ�꣬������ֹͣ�˶��ı��

					//�����ѧͳ�Ʋ���
					if (interior_params_p->bStat_flag == TRUE)
					{
						interior_params_p->stu_statistics.moveCount++;
					}
				}
				stuTrack_allState[i] = stuTrack_allState[--(interior_params_p->count_trackObj_allState)];
				i--;
				continue;
			}
		}

		if (_time > THRESHOLD_STURECK_MOVE_STOP_TIME)
		{
			if (stuTrack_allState[i].flag_state == RESULT_STUTRACK_MOVE_FLAG
				|| stuTrack_allState[i].flag_state == STATE_STUTRACK_BIG_FLAG)
			{
				interior_params_p->result_flag |= RESULT_STUTRACK_STOPMOVE_FLAG;	//���֮ǰ���˶���Ŀ�꣬������ֹͣ�˶��ı��
				//�����ѧͳ�Ʋ���
				if (interior_params_p->bStat_flag == TRUE)
				{
					interior_params_p->stu_statistics.moveCount++;
				}

				stuTrack_allState[i].flag_state = STATE_STUTRACK_NULL_FLAG;			//��Ϊ��Ŀ���Ѿ�ͣ������
				stuTrack_allState[i].origin_position = stuTrack_allState[i].current_position;
				continue;
			}
		}

		if (!(stuTrack_allState[i].current_position.y < interior_params_p->rangeStart || stuTrack_allState[i].current_position.y > interior_params_p->rangeEnd)
			&& !(stuTrack_allState[i].current_position.x < interior_params_p->rangeStartX || stuTrack_allState[i].current_position.x > interior_params_p->rangeEndX))//�ж��Ƿ��ڷ�Χ��
		{
			//���㼸����ǰ�뵱ǰ֡�Ĳ�ֵ
			Track_Rect_t _roi = stuTrack_allState[i].roi;//���㷶Χ
			if (stuTrack_allState[i].flag_state != STATE_STUTRACK_STANDUP_FLAG)
			{
				_roi.x += (_roi.width >> 5);
				_roi.width -= (_roi.width >> 4);
				_roi.height = ITC_IMIN(stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
			}
			float diffThreshold = _roi.width*_roi.height;
			int oldBKid = interior_params_p->current_BKid - OLDBK_STURECK_IMG_FREAM_ID;//��ȡ����ǰ��ͼ���뵱ǰͼ����жԱ�
			if (oldBKid < 0)
			{
				oldBKid += SATUTRACK_BK_IMG_COUNT;
			}
			float frameDiffVal = (track_sub_matROI(interior_params_p->currMat, interior_params_p->BKmat[oldBKid], _roi)) / diffThreshold;

			if (stuTrack_allState[i].flag_matching != TRUE)
			{
				//��ǰû��ƥ�䵽��ֱ����ԭλ�ü����˶�����
				int flag_ROI = track_calculateDirect_ROI(interior_params_p->mhiMat, stuTrack_allState[i].roi, &direct, &dX, &dY);
				int standard_direct = stuTrack_direct_threshold[stuTrack_allState[i].roi.x + (stuTrack_allState[i].roi.width >> 1)];
				int standard_direct_Dir = (standard_direct > ITC_180DEGREE) ? (standard_direct - ITC_180DEGREE) : (standard_direct + ITC_180DEGREE);
				if (stuTrack_allState[i].flag_state != STATE_STUTRACK_STANDUP_FLAG)
				{
					if (abs(standard_direct - direct) <= stuTrack_direct_range && flag_ROI == 1)
					{
						if (frameDiffVal > THRESHOLD_STURECK_HISTOGRAM_Dvalue)
						{
							stuTrack_allState[i].count_up++;
						}
					}
					else
					{
						stuTrack_allState[i].count_up = stuTrack_allState[i].count_up > 0 ? stuTrack_allState[i].count_up - 1 : 0;
					}
				}
				else
				{
					if (abs(standard_direct_Dir - direct) <= stuTrack_direct_range && flag_ROI == 1)
					{
						if (frameDiffVal > THRESHOLD_STURECK_HISTOGRAM_Dvalue)
						{
							stuTrack_allState[i].count_down++;
						}
					}
					else
					{
						stuTrack_allState[i].count_down = stuTrack_allState[i].count_down > 0 ? stuTrack_allState[i].count_down - 1 : 0;
					}
				}
				if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_3)
				{
					_PRINTF("angle��%d,standard_direct��%d-%d\n", direct, standard_direct, standard_direct_Dir);
				}
			}
			stuTrack_allState[i].flag_matching = FALSE;	//���ƥ��ɹ����

			if (stuTrack_allState[i].flag_state == STATE_STUTRACK_NULL_FLAG || stuTrack_allState[i].flag_state == STATE_STUTRACK_SITDOWN_FLAG)//�ж��Ƿ��Ѿ�������Ŀ��
			{
				//�����ƶ�����
				int diff_x = abs(stuTrack_allState[i].origin_position.x - stuTrack_allState[i].current_position.x);
				int diff_y = abs(stuTrack_allState[i].origin_position.y - stuTrack_allState[i].current_position.y);
				int centre_y = stuTrack_allState[i].roi.y + (stuTrack_allState[i].roi.height >> 1);
				int size_threshold = stuTrack_size_threshold[centre_y] + stuTrack_size_threshold[centre_y];

				int topy_diff = stuTrack_allState[i].origin_top_y - stuTrack_allState[i].roi.y;
				double topy_diffThreshold = stuTrack_size_threshold[stuTrack_allState[i].origin_top_y] * stuTrack_standup_threshold;
				int flag1 = stuTrack_allState[i].count_up > stuTrack_standCount_threshold && topy_diff > topy_diffThreshold;
				int flag2 = stuTrack_allState[i].count_up > 0 && topy_diff > (topy_diffThreshold * EXPADN_STURECK_STANDUP_HEIGHT_PARAMS);

				if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_3)
				{
					_PRINTF("analyzeID:%d,count_up:%d,count_down��%d,flag_state��%d,position:%d,%d,Histogram diff:%f,origin_top_y:%d,roi.y:%d;topy_diffThreshold:%lf;diff_xy:%d,%d;size_threshold:%d\n", i, stuTrack_allState[i].count_up, stuTrack_allState[i].count_down, stuTrack_allState[i].flag_state, stuTrack_allState[i].current_position.x, stuTrack_allState[i].current_position.y, frameDiffVal, stuTrack_allState[i].origin_top_y, stuTrack_allState[i].roi.y, topy_diffThreshold, diff_x, diff_y, size_threshold);
				}

				if ((flag1 || flag2)
					&& frameDiffVal > THRESHOLD_STURECK_HISTOGRAM_UP_Dvalue
					&& diff_x < size_threshold
					&& diff_y < size_threshold*2
					)
				{
					if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
					{
						_PRINTF("!!!!!!!!!!!!!!!!!!!stand up��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].current_position.x, stuTrack_allState[i].current_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
					}
					//����Ϊ����
					stuTrack_allState[i].count_up = 0;
					stuTrack_allState[i].count_down = 0;
					stuTrack_allState[i].upstand_tClock = gettime();
					int par_width = stuTrack_size_threshold[centre_y] + (stuTrack_size_threshold[centre_y] >> 2);
					if (stuTrack_allState[i].roi.width > par_width)
					{
						int centre_x = stuTrack_allState[i].roi.x + (stuTrack_allState[i].roi.width >> 1);//�������ĵ�
						stuTrack_allState[i].roi.x = centre_x - (par_width >> 1);//ȡpar_width���ֵ�󣬼����µ�x�����
						stuTrack_allState[i].roi.width = par_width;				//ȡpar_width���ֵ
					}
					stuTrack_allState[i].roi.height = ITC_IMIN(stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
					stuTrack_allState[i].standUp_size = stuTrack_allState[i].roi.width*stuTrack_allState[i].roi.height*stuTrack_standTomove_threshold;
					stuTrack_allState[i].standUp_position = stuTrack_allState[i].current_position;		//��¼������λ��
					stuTrack_allState[i].origin_top_y = stuTrack_allState[i].roi.y;
					stuTrack_allState[i].topy_diffThreshold = topy_diffThreshold;
					interior_params_p->result_flag |= RESULT_STUTRACK_STANDUP_FLAG;
					stuTrack_allState[i].flag_state = STATE_STUTRACK_STANDUP_FLAG;
				}

				if (stuTrack_allState[i].flag_state == STATE_STUTRACK_NULL_FLAG || stuTrack_allState[i].flag_state == STATE_STUTRACK_SITDOWN_FLAG)
				{
					//�����������ж��Ƿ����ƶ�Ŀ��
					if (diff_x > stuTrack_allState[i].dis_threshold || diff_y > stuTrack_allState[i].dis_threshold + stuTrack_size_threshold[centre_y])
					{
						_time = stuTrack_allState[i].current_tClock - stuTrack_allState[i].start_tClock;
						if (_time > THRESHOLD_STURECK_MOVETIME_DELETE_TIME)
						{
							if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
							{
								_PRINTF("!!!!!!!!!!!!!!!!!!!find Move��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].origin_position.x, stuTrack_allState[i].origin_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
							}
							stuTrack_allState[i].moveBig_tClock = gettime();
							interior_params_p->result_flag |= RESULT_STUTRACK_MOVE_FLAG;//�����ƶ�Ŀ��ı��
							stuTrack_allState[i].flag_state = STATE_STUTRACK_MOVE_FLAG;
						}
					}
					else
					{
						//��Ŀ��
						if ((stuTrack_allState[i].roi.width > size_threshold && (stuTrack_allState[i].roi.height > size_threshold + stuTrack_size_threshold[centre_y])))
						{
							if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
							{
								_PRINTF("!!!!!!!!!!!!!!!!!!!find big ��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].origin_position.x, stuTrack_allState[i].origin_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
							}
							stuTrack_allState[i].moveBig_tClock = gettime();
							interior_params_p->result_flag |= RESULT_STUTRACK_MOVE_FLAG;//�����ƶ�Ŀ��ı��
							stuTrack_allState[i].flag_state = STATE_STUTRACK_BIG_FLAG;
						}
					}
				}	//end if (stuTrack_allState[i].flag_state == STATE_STUTRACK_NULL_FLAG || stuTrack_allState[i].flag_state == STATE_STUTRACK_SITDOWN_FLAG)
			}
			else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_STANDUP_FLAG)
			{
				//�˴��Ƕ�վ��Ŀ����д���
				//����������Ѿ��������ж��Ƿ�����
				if ((stuTrack_allState[i].count_down > stuTrack_sitdownCount_threshold || (stuTrack_allState[i].roi.y - stuTrack_allState[i].origin_top_y)>stuTrack_allState[i].topy_diffThreshold)
					&& frameDiffVal > THRESHOLD_STURECK_HISTOGRAM_DOWN_Dvalue
					)
				{
					if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
					{
						_PRINTF("!!!!!!!!!!!!!!!!!!!sit down��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].current_position.x, stuTrack_allState[i].current_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
					}
					stuTrack_allState[i].count_up = 0;
					stuTrack_allState[i].count_down = 0;
					stuTrack_allState[i].origin_top_y = stuTrack_allState[i].roi.y;
					interior_params_p->result_flag |= RESULT_STUTRACK_SITDOWN_FLAG;
					stuTrack_allState[i].flag_state = STATE_STUTRACK_SITDOWN_FLAG;

					//�����ѧͳ�Ʋ���
					if (interior_params_p->bStat_flag == TRUE)
					{
						interior_params_p->standUpStatTemp.start = stuTrack_allState[i].upstand_tClock;
						interior_params_p->standUpStatTemp.end = gettime();
						interior_params_p->standUpStatTemp.deltatime = interior_params_p->standUpStatTemp.end - interior_params_p->standUpStatTemp.start;
						interior_params_p->standUpStatTemp.next = NULL;
						track_timerIncrease(&(interior_params_p->stu_statistics.standUpTimer), &(interior_params_p->standUpStatTemp));
					}
				}
				else
				{
					//û�����£��ж��Ƿ��ƶ�����
					int move_threshold = stuTrack_allState[i].standUp_size;
					int diff_x = stuTrack_allState[i].standUp_position.x - stuTrack_allState[i].current_position.x;
					int diff_y = stuTrack_allState[i].standUp_position.y - stuTrack_allState[i].current_position.y;
					diff_x *= diff_x;
					diff_y *= diff_y;
					int centre_y = stuTrack_allState[i].roi.y + stuTrack_allState[i].roi.height;
					int size_threshold = stuTrack_size_threshold[centre_y] + stuTrack_size_threshold[centre_y];
					if (diff_x > move_threshold || diff_y > move_threshold)
					{
						if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
						{
							_PRINTF("!!!!!!!!!!!!!!!!!!!stand to Move��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].current_position.x, stuTrack_allState[i].current_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
						}
						//����ƶ�Ŀ��
						stuTrack_allState[i].count_up = 0;
						stuTrack_allState[i].count_down = 0;
						interior_params_p->result_flag |= RESULT_STUTRACK_MOVE_FLAG;//�����ƶ�Ŀ��ı��
						stuTrack_allState[i].flag_state = STATE_STUTRACK_MOVE_FLAG;
						stuTrack_allState[i].moveBig_tClock = stuTrack_allState[i].upstand_tClock;
					}
					else if (stuTrack_allState[i].roi.width > size_threshold)
					{
						if (interior_params_p->stuTrack_debugMsg_flag >= SATUTRACK_PRINTF_LEVEL_2)
						{
							_PRINTF("!!!!!!!!!!!!!!!!!!!find big ��origin:%d,%d,size:%d,%d\n", stuTrack_allState[i].origin_position.x, stuTrack_allState[i].origin_position.y, stuTrack_allState[i].roi.width, stuTrack_allState[i].roi.height);
						}
						stuTrack_allState[i].count_up = 0;
						stuTrack_allState[i].count_down = 0;
						interior_params_p->result_flag |= RESULT_STUTRACK_MOVE_FLAG;//�����ƶ�Ŀ��ı��
						stuTrack_allState[i].flag_state = STATE_STUTRACK_BIG_FLAG;
						stuTrack_allState[i].moveBig_tClock = stuTrack_allState[i].upstand_tClock;
					}
				}
			} //end if(stuTrack_allState[i].flag_state != STATE_STUTRACK_STANDUP_FLAG)
		}//�ж��Ƿ��ڷ�Χ��
		else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_STANDUP_FLAG)
		{
			//���ڷ�Χ������վ��Ŀ�ֱ꣬��ɾ��
			int k = 0;
			(interior_params_p->count_trackObj_allState)--;
			for (k = i; k < interior_params_p->count_trackObj_allState; k++)
			{
				stuTrack_allState[k] = stuTrack_allState[k + 1];
			}
			i--;
			continue;
		}
	}
}

static void stuTrack_proStandDown_ROI(StuITRACK_InteriorParams* interior_params_p)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_proStandDown_ROI\n");
	//ƥ��ͷ�����ѡ����
	int i = 0;
	for (i = 0; i < interior_params_p->count_stuTrack_rect; i++)
	{
		stuTrack_matchingSatnd_ROI(interior_params_p, interior_params_p->stuTrack_rect_arr[i]);
	}
	stuTrack_analyze_ROI(interior_params_p);			//������ѡroi
}

static void stuTrack_drawShow_imgData(StuITRACK_InteriorParams* interior_params_p, StuITRACK_OutParams_t* return_params, itc_uchar* imageData, itc_uchar* bufferuv)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_drawShow_imgData\n");
	if (interior_params_p->stuTrack_Draw_flag == FALSE || bufferuv == NULL)
	{	
		return;
	}

	//�������
	int i = 0;
	Track_Size_t *srcimg_size = &interior_params_p->srcimg_size;	//ԭʼͼ���С
	StuTrack_allState_t* stuTrack_allState = interior_params_p->stuTrack_allState;

	Track_Rect_t *rect;
	Track_Point_t *origin_position;

#ifdef _WIN32
	int YUV420_type = TRACK_DRAW_YUV420P;
#else
	int YUV420_type = TRACK_DRAW_YUV420SP;
#endif

#define CALSS_CUONT_STUTRACK_RESULT_DRAW 8
#define WHIDTH_STUTRACK_RESULT_DRAW 13
#define HEIGHT_STUTRACK_RESULT_DRAW 13
	Track_Rect_t returnParams_rect;
	int x_step = interior_params_p->img_size.width / CALSS_CUONT_STUTRACK_RESULT_DRAW;
	returnParams_rect.x = x_step;
	returnParams_rect.y = 2;
	returnParams_rect.width = WHIDTH_STUTRACK_RESULT_DRAW;
	returnParams_rect.height = HEIGHT_STUTRACK_RESULT_DRAW;
	if (return_params->result_flag == RESULT_STUTRAKC_NULL_CAMERA)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->lilac_colour, YUV420_type);
		//_PRINTF("����ѧ�����棡");
	}
	returnParams_rect.x += x_step;
	if ((return_params->result_flag&RESULT_STUTRAKC_MOVE_CAMERA) != 0)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->yellow_colour, YUV420_type);
		//_PRINTF("�ƶ���ͷ��");
	}
	returnParams_rect.x += x_step;
	if ((return_params->result_flag&RESULT_STUTRAKC_FEATURE_CAMERA) != 0)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->red_colour, YUV420_type);
		//_PRINTF("ѧ����д��");
	}
	returnParams_rect.x += x_step;
	if ((return_params->result_flag&RESULT_STUTRAKC_PAN0RAMA_CAMERA) != 0)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->green_colour, YUV420_type);
		//_PRINTF("ѧ��ȫ����");
	}
	returnParams_rect.x += x_step;
	if ((return_params->result_flag&RESULT_STUTRAKC_noTCH_FEATURE_CAMERA) != 0)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->red_colour, YUV420_type);
		//_PRINTF("��ʦ���ڽ�̨��ѧ����д��");
	}
	returnParams_rect.x += x_step;
	if ((return_params->result_flag&RESULT_STUTRAKC_noTCH_PAN0RAMA_CAMERA) != 0)
	{
		track_draw_rectangle(imageData, bufferuv, srcimg_size, &returnParams_rect, &interior_params_p->green_colour, YUV420_type);
		//_PRINTF("��ʦ�޶���ʱ��ѧ��ȫ����");
	}

	//���Ƽ������
	Track_Rect_t range_rect;
	range_rect.x = interior_params_p->rangeStartX;
	range_rect.y = interior_params_p->rangeStart;
	range_rect.width = interior_params_p->rangeEndX - interior_params_p->rangeStartX;
	range_rect.height = interior_params_p->rangeEnd - interior_params_p->rangeStart;
	track_draw_rectangle(imageData, bufferuv, srcimg_size, &range_rect, &interior_params_p->green_colour, YUV420_type);

	for (i = 0; i < interior_params_p->count_trackObj_allState; i++)
	{
		origin_position = &(stuTrack_allState[i].origin_position);
		rect = &(stuTrack_allState[i].roi);
		if (stuTrack_allState[i].flag_state != STATE_STUTRACK_NULL_FLAG)
		{
			if (stuTrack_allState[i].flag_state == STATE_STUTRACK_STANDUP_FLAG)
			{
				track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->red_colour, YUV420_type);
			}
			else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_SITDOWN_FLAG)
			{
				track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->yellow_colour, YUV420_type);
			}
			else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_MOVE_FLAG)
			{
				track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->pink_colour, YUV420_type);
			}
			else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_BIG_FLAG)
			{
				track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->blue_colour, YUV420_type);
			}
			Track_Point_t *current_position = &(stuTrack_allState[i].current_position);
			track_draw_line(imageData, bufferuv, srcimg_size, origin_position, current_position, &interior_params_p->green_colour, YUV420_type);
			track_draw_point(imageData, bufferuv, srcimg_size, origin_position, &interior_params_p->red_colour, YUV420_type);
		}
		else
		{
			track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->lilac_colour, YUV420_type);
		}
	}

	//Track_Rect_t *stuTrack_rect_arr = interior_params_p->stuTrack_rect_arr;
	//for (i = 0; i < interior_params_p->count_stuTrack_rect; i++)
	//{
	//	int direct;
	//	float dX = 0.0, dY = 0.0;
	//	track_calculateDirect_ROI((Itc_Mat_t *)interior_params_p->mhiMat, stuTrack_rect_arr[i], &direct, &dX, &dY);
	//	//_PRINTF("�Ƕȣ�%d\n", direct);
	//	//int x1 = (int)10 * cos(direct*ITC_ANGLE_TO_RADIAN);
	//	//int y1 = (int)10 * sin(direct*ITC_ANGLE_TO_RADIAN);
	//	int x1 = (int)10 * dX;
	//	int y1 = (int)10 * dY;
	//	Track_Point_t pt1 = { 0, 0 };
	//	pt1.x = stuTrack_rect_arr[i].x + stuTrack_rect_arr[i].width / 2;
	//	pt1.y = stuTrack_rect_arr[i].y + stuTrack_rect_arr[i].height / 2;
	//	Track_Point_t pt2 = { 0, 0 };
	//	pt2.x = pt1.x + x1;
	//	pt2.y = pt1.y + y1;
	//	//rect = &(stuTrack_rect_arr[i]);
	//	//track_draw_rectangle(imageData, bufferuv, srcimg_size, rect, &interior_params_p->lilac_colour, YUV420_type);
	//	track_draw_line(imageData, bufferuv, srcimg_size, &pt1, &pt2, &interior_params_p->green_colour, YUV420_type);
	//	track_draw_point(imageData, bufferuv, srcimg_size, &pt1, &interior_params_p->red_colour, YUV420_type);
	//}
}

#define THRESHOLD_STURECK_DIS_JUDGE 50
static int distance_judge(StuITRACK_InteriorParams* interior_params_p, TrackPrarms_Point_t pt1, TrackPrarms_Point_t pt2, int  dis_threshold)
{
	int dis_x = abs(pt1.x - pt2.x);
	int dis_y = abs(pt1.y - pt2.y);
	if (dis_x<dis_threshold && dis_y<dis_threshold)
	{
		//_PRINTF("���ƶ����룺%d,%d\n", dis_x, dis_y);
		return TRUE;
	}
	else
	{
		//_PRINTF("�ƶ����룺%d,%d\n", dis_x, dis_y);
		return FALSE; 
	}
}

#define THRESHOLD_STURECK_UPCAST_TIME			2000			//�ޱ仯ʱ����״̬��Ϣ��ʱ����
#define THRESHOLD_STURECK_MOVECAMERA_TIME		500				//��������ƶ��źŵ���Сʱ����
#define THRESHOLD_STURECK_MOVECAMERA_CUT_TIME	2000			//�ƶ��źŷ��ͺ�ȴ������λ�л���д��ʱ��
//#define THRESHOLD_STURECK_STOPMOVE_CUT_TIME		3000			//�ƶ�Ŀ��ֹͣ�󣬸��źű�����ʱ��
#define THRESHOLD_STURECK_MOVE_SUSTIAN_TIME		2000			//�ƶ�Ŀ����Ҫ������ʱ�䣬�������ʱ��Ŵ���
#define NORMALIZE_STURECK_ZOOM_PARAM			1000			//����ϵ���ֵ����������ķֳ�1000��2000��3000����
static stuTrackReturn stuTrack_reslut(StuITRACK_InteriorParams* interior_params_p, StuITRACK_OutParams_t* return_params)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_reslut\n");

	stuTrackReturn flag_return = RETURN_STUTRACK_noNEED_PROCESS;

	int count_trackObj_allState = 0;
	int count_trackObj_stand = 0;		//ͳ���ƶ�Ŀ�����
	int count_trackObj_bigMove = 0;		//ͳ������Ŀ�����
	int i = 0;
	int id_stuUp = 0;
	int id_stuMove = 0;
	unsigned int _time = gettime();
	unsigned int _timePioneerMove = 0;
	//�������µ�����Ŀ��λ��
	Track_Point_t pt = { 0, 0 };
	Track_Point_t inpt = { 0, 0 };
	int lowest_y = 0;
	StuTrack_allState_t* stuTrack_allState = interior_params_p->stuTrack_allState;
	for (i = 0; i < interior_params_p->count_trackObj_allState; i++)
	{
		if (stuTrack_allState[i].flag_state != STATE_STUTRACK_NULL_FLAG)
		{
			if (stuTrack_allState[i].flag_state == STATE_STUTRACK_STANDUP_FLAG)
			{
				id_stuUp = i;
				count_trackObj_stand++;
			}
			else if (stuTrack_allState[i].flag_state == STATE_STUTRACK_MOVE_FLAG || stuTrack_allState[i].flag_state == STATE_STUTRACK_BIG_FLAG)
			{	
				//ѡȡ���緢�ֵ��ƶ�Ŀ�꣬�����뵱ǰʱ��ļ��
				unsigned int tempT = _time - stuTrack_allState[i].start_tClock;
				if (_timePioneerMove < tempT)
				{	
					id_stuMove = i;
					_timePioneerMove = tempT;
				}
				count_trackObj_bigMove++;
			}
			if (lowest_y < stuTrack_allState[i].roi.y + stuTrack_allState[i].roi.height)
			{	
				//��һ����͵�Ŀ��
				lowest_y = stuTrack_allState[i].roi.y + stuTrack_allState[i].roi.height;
			}
			count_trackObj_allState++;
		}
	}

	if (count_trackObj_stand > 0)
	{
		inpt.x = interior_params_p->stuTrack_allState[id_stuUp].roi.x + (interior_params_p->stuTrack_allState[id_stuUp].roi.width >> 1);
		inpt.y = interior_params_p->stuTrack_allState[id_stuUp].roi.y + (interior_params_p->stuTrack_allState[id_stuUp].roi.height >> 1);
		perspectiveConvert(&inpt, &pt, interior_params_p->transformationMatrix);
		int size = interior_params_p->stuTrack_allState[id_stuUp].roi.width;
		size = (int)(interior_params_p->stretchingAB[0] * size + interior_params_p->stretchingAB[1]);

		interior_params_p->old_standup_position.x = pt.x;
		interior_params_p->old_standup_position.y = pt.y;
		interior_params_p->old_standup_stretchingCoefficient = size > 0 ? (size - (size% NORMALIZE_STURECK_ZOOM_PARAM)) : 0;
	}
	if (count_trackObj_bigMove>0)
	{
		//�������µ��ƶ�Ŀ��λ��
		inpt.x = interior_params_p->stuTrack_allState[id_stuMove].roi.x + (interior_params_p->stuTrack_allState[id_stuMove].roi.width >> 1);
		inpt.y = interior_params_p->stuTrack_allState[id_stuMove].roi.y + (interior_params_p->stuTrack_allState[id_stuUp].roi.height >> 3);
		perspectiveConvert(&inpt, &pt, interior_params_p->transformationMatrix);
		int size = interior_params_p->stuTrack_allState[id_stuUp].roi.width;
		size = (int)(interior_params_p->stretchingAB[0] * size + interior_params_p->stretchingAB[1]);

		interior_params_p->old_move_position.x = pt.x;
		interior_params_p->old_move_position.y = pt.y;
		interior_params_p->old_move_stretchingCoefficient = size > 0 ? (size - (size% NORMALIZE_STURECK_ZOOM_PARAM)) : 0;
	}

	return_params->result_flag = interior_params_p->OldResult_flag;
	//����
	if (count_trackObj_stand>0 || count_trackObj_bigMove > 0)
	{
		interior_params_p->old_move_Stopflag = FALSE;//�����ƶ�Ŀ��ֹͣ�ı��
		if (interior_params_p->result_flag == RESULT_STUTRACK_NULL_FLAG		//û��״̬�仯��ʱ�������д
			&& count_trackObj_bigMove == 0									//����û���ƶ�Ŀ���ʱ��
			&& interior_params_p->move_csucceed_flag == TRUE)				//�����ȷ������ƶ���ͷ����Ϣ
		{
			//֮ǰ��ȫ��
			if (count_trackObj_stand == 1)
			{
				//��ѧ��������д
				if ((_time - interior_params_p->move_camera_time) > THRESHOLD_STURECK_MOVECAMERA_CUT_TIME)
				{
					return_params->result_flag = RESULT_STUTRAKC_FEATURE_CAMERA;
				}		
			}
		}
		else
		{
			//�ж��Ƿ���Ҫ��ȫ�����ƶ���д��ͷ
			if (count_trackObj_stand > 0 &&count_trackObj_bigMove == 0)
			{
				interior_params_p->move_csucceed_flag = FALSE;
				return_params->result_flag = RESULT_STUTRAKC_PAN0RAMA_CAMERA;			//��ѧ��ȫ��
				if(count_trackObj_stand == 1)
				{
					//ֻ��һ������Ŀ��Ŵ���
					if ((_time - interior_params_p->move_camera_time) > THRESHOLD_STURECK_MOVECAMERA_TIME)
					{
						interior_params_p->move_camera_time = _time;
						interior_params_p->move_csucceed_flag = TRUE;

						return_params->position = interior_params_p->old_standup_position;
						return_params->stretchingCoefficient = interior_params_p->old_standup_stretchingCoefficient;
						return_params->result_flag |= RESULT_STUTRAKC_MOVE_CAMERA;		//�ƶ���д��ͷ	
					}
				}
			}
			else if (count_trackObj_bigMove == 1 && count_trackObj_stand == 0)
			{
				//���ƶ�Ŀ��
				if (_timePioneerMove > THRESHOLD_STURECK_MOVE_SUSTIAN_TIME)
				{
					return_params->result_flag = RESULT_STUTRAKC_PAN0RAMA_CAMERA;		//��ѧ��ȫ��
				}
				else
				{
					return_params->result_flag = RESULT_STUTRAKC_noTCH_PAN0RAMA_CAMERA;		//�����̨û����ʦ������ѧ��ȫ��
				}

				if (distance_judge(interior_params_p, return_params->position, interior_params_p->old_move_position, THRESHOLD_STURECK_DIS_JUDGE) == FALSE)
				{
					interior_params_p->move_csucceed_flag = FALSE;
					if ((_time - interior_params_p->move_camera_time) > THRESHOLD_STURECK_MOVECAMERA_TIME)
					{
						interior_params_p->move_camera_time = _time;
						interior_params_p->move_csucceed_flag = TRUE;

						return_params->position = interior_params_p->old_move_position;
						return_params->stretchingCoefficient = interior_params_p->old_move_stretchingCoefficient;
						return_params->result_flag |= RESULT_STUTRAKC_MOVE_CAMERA;			//�ƶ���д��ͷ	
					}
				}
			}
			else
			{
				//�����������ʱ�ƶ�Ŀ��϶���Ϊ0
				if (_timePioneerMove > THRESHOLD_STURECK_MOVE_SUSTIAN_TIME)
				{
					return_params->result_flag = RESULT_STUTRAKC_PAN0RAMA_CAMERA;		//��ѧ��ȫ��
				}
				else
				{
					return_params->result_flag = RESULT_STUTRAKC_noTCH_PAN0RAMA_CAMERA;		//�����̨û����ʦ������ѧ��ȫ��
				}
			}
		}
	}
	else if (RESULT_STUTRACK_IF_STOPMOVE(interior_params_p->result_flag))
	{
		//����˴�˵����ǰ�Ѿ�û���κ��˶�Ŀ����(�к�ѡĿ��)
		interior_params_p->old_move_Stopflag = TRUE;//���ƶ�Ŀ��ͣ��,���ñ��
		interior_params_p->moveStopObj_time = _time;//��¼��ʱ��
		return_params->result_flag = RESULT_STUTRAKC_noTCH_PAN0RAMA_CAMERA;		//�����̨û����ʦ������ѧ��ȫ��
		if (distance_judge(interior_params_p, return_params->position, interior_params_p->old_move_position, THRESHOLD_STURECK_DIS_JUDGE) == FALSE)
		{
			//��һ���ƶ��ľ�ͷ������λ�����һ������ʱ���ƶ���ͷ
			interior_params_p->move_csucceed_flag = FALSE;
			if ((_time - interior_params_p->move_camera_time) > (THRESHOLD_STURECK_MOVECAMERA_TIME >> 1))
			{
				interior_params_p->move_camera_time = _time;
				interior_params_p->move_csucceed_flag = TRUE;

				return_params->position = interior_params_p->old_move_position;
				return_params->stretchingCoefficient = interior_params_p->old_move_stretchingCoefficient;
				return_params->result_flag |= RESULT_STUTRAKC_MOVE_CAMERA;			//�ƶ���д��ͷ
			}
		}
		else
		{
			//����Ҫ�ƶ���ͷ
			if (_time - interior_params_p->move_camera_time > (THRESHOLD_STURECK_MOVECAMERA_CUT_TIME >> 1))
			{
				return_params->result_flag = RESULT_STUTRAKC_noTCH_FEATURE_CAMERA;//�����̨û����ʦ����ѧ����д
			}
		}
	}
	else if (interior_params_p->old_move_Stopflag == TRUE)
	{
		if (interior_params_p->move_csucceed_flag == TRUE)
		{
			if (_time - interior_params_p->move_camera_time > (THRESHOLD_STURECK_MOVECAMERA_CUT_TIME >> 1))
			{
				if (_time - interior_params_p->moveStopObj_time > 2500)//��������2.5�룬�����øñ��
				{
					interior_params_p->old_move_Stopflag = FALSE;	//�����ƶ�Ŀ��ֹͣ�ı��
				}
				return_params->result_flag = RESULT_STUTRAKC_noTCH_FEATURE_CAMERA;//�����̨û����ʦ����ѧ����д
			}
		}
		else
		{
			//��һ�����ƾ�ͷû�Ƶ�������
			if ((_time - interior_params_p->move_camera_time) > (THRESHOLD_STURECK_MOVECAMERA_TIME >> 1))
			{
				interior_params_p->move_camera_time = _time;
				interior_params_p->move_csucceed_flag = TRUE;

				return_params->position = interior_params_p->old_move_position;
				return_params->stretchingCoefficient = interior_params_p->old_move_stretchingCoefficient;
				return_params->result_flag |= RESULT_STUTRAKC_MOVE_CAMERA;			//�ƶ���д��ͷ
			}
		}
	}
	else
	{
		if (count_trackObj_allState > 0						//�к�ѡĿ��
			&& lowest_y>interior_params_p->rangeEnd			//���ڵײ��Ĳſ�����Ϊ����ʦ
			)
		{	
			_PRINTF("�ײ��к�ѡĿ��");
			return_params->result_flag = RESULT_STUTRAKC_noTCH_PAN0RAMA_CAMERA;		//�����̨û����ʦ������ѧ��ȫ��
		}
		else
		{
			return_params->result_flag = RESULT_STUTRAKC_NULL_CAMERA;
		}
	}

	if ((return_params->result_flag&(~RESULT_STUTRAKC_MOVE_CAMERA)) != (interior_params_p->OldResult_flag&(~RESULT_STUTRAKC_MOVE_CAMERA))
		|| (return_params->result_flag&RESULT_STUTRAKC_MOVE_CAMERA) != 0)
	{
		//��Ҫ���״���
		flag_return = RETURN_STUTRACK_NEED_PROCESS;
		interior_params_p->last_upcast_time = _time;
		interior_params_p->OldResult_flag = (return_params->result_flag&(~RESULT_STUTRAKC_MOVE_CAMERA));	//�ѵ�ǰ״̬����(ֻ�����л����״̬���������ƶ���ͷ��״̬)
	}
	else if (_time - interior_params_p->last_upcast_time > THRESHOLD_STURECK_UPCAST_TIME)
	{
		//ûҪ�仯��ʱ��ʱ����
		flag_return = RETURN_STUTRACK_NEED_PROCESS;
		interior_params_p->last_upcast_time = _time;
	}

	return flag_return;
}

static void stuTrack_Copy_matData(StuITRACK_InteriorParams* interior_params_p, itc_uchar* srcData)
{
	ITC_FUNCNAME("FUNCNAME:stuTrack_resizeCopy_matData\n");
	int y = 0;
	int height = interior_params_p->img_size.height;
	int dst_step = interior_params_p->img_size.width;
	int src_step = interior_params_p->srcimg_size.width;
	if (dst_step > src_step)
	{
		_PRINTF("The image cache size error!\n");
		return;
	}

	itc_uchar* dst_p = interior_params_p->currMat->data.ptr;
	for (y = 0; y < height; y++)
	{
		memcpy(dst_p, srcData, sizeof(itc_uchar)* dst_step);
		dst_p += dst_step;
		srcData += src_step;
	}
}
#define CHECH_STURRACK_RESULT_OK 0
static int stuTrack_check_clientParams(StuITRACK_ClientParams_t* clientParams_p)
{
	if (clientParams_p->height <= 0 || clientParams_p->width <= 0)
	{
		clientParams_p->width = WIDTH_STUTRACK_IMG_;
		clientParams_p->height = HEIGHT_STUTRACK_IMG_;
	}

	if (clientParams_p->stuTrack_debugMsg_flag < 0)
	{
		clientParams_p->stuTrack_debugMsg_flag = 0;
	}

	if (clientParams_p->stuTrack_direct_range <= 0)
	{
		//����ʱ����ĽǶ�ƫ�뷶Χ
		clientParams_p->stuTrack_direct_range = RANGE_STUTRACK_STANDDIRECT_DEFALUT_PARAMS;
	}

	if (clientParams_p->stuTrack_standCount_threshold <= 0)
	{
		//��������
		clientParams_p->stuTrack_standCount_threshold = THRESHOLD_STUTRACK_STANDCOUNT_DEFALUT_PARAMS;
	}

	if (clientParams_p->stuTrack_sitdownCount_threshold <= 0)
	{
		//���¼���
		clientParams_p->stuTrack_sitdownCount_threshold = THRESHOLD_STUTRACK_SITDOWNCOUNT_DEFALUT_PARAMS;
	}

	if (clientParams_p->stuTrack_moveDelayed_threshold <= 0)
	{
		//�ƶ�Ŀ�걣�ָ��ٵ���ʱ���������ʱ�����˶������������(��λ������)
		clientParams_p->stuTrack_moveDelayed_threshold = THRESHOLD_STUTRACK_MOVEDELAYED_DEFALUT_TIME;
	}

	if (clientParams_p->stuTrack_deleteTime_threshold <= 0)
	{
		//Ŀ�걣�ָ��ٵ��ʱ��ʱ���������ʱ��ɾ��Ŀ��(��λ����)
		clientParams_p->stuTrack_deleteTime_threshold = THRESHOLD_STURECK_ALL_DELETE_TIME / 1000;
	}

	if ((clientParams_p->stuTrack_move_threshold - 0.1) <= 0)
	{
		//�ƶ���ֵ
		clientParams_p->stuTrack_move_threshold = THRESHOLD_STUTRACK_MOVE_DEFALUT_PARAMS;
	}

	if ((clientParams_p->stuTrack_standup_threshold - 0.1) <= 0)
	{
		//������ֵ
		clientParams_p->stuTrack_standup_threshold = THRESHOLD_STUTRACK_STANDUP_DEFALUT_PARAMS;
	}

	if (clientParams_p->stuTrack_vertex[0].x < 0 ||
		clientParams_p->stuTrack_vertex[1].x < 0 ||
		clientParams_p->stuTrack_vertex[2].x < 0 ||
		clientParams_p->stuTrack_vertex[3].x < 0 ||
		clientParams_p->stuTrack_vertex[0].y < 0 ||
		clientParams_p->stuTrack_vertex[1].y < 0 ||
		clientParams_p->stuTrack_vertex[2].y < 0 ||
		clientParams_p->stuTrack_vertex[3].y < 0)
	{
		clientParams_p->stuTrack_vertex[0].x = 0;
		clientParams_p->stuTrack_vertex[0].y = 0;
		clientParams_p->stuTrack_vertex[1].x = clientParams_p->width;
		clientParams_p->stuTrack_vertex[1].y = 0;
		clientParams_p->stuTrack_vertex[2].x = clientParams_p->width;
		clientParams_p->stuTrack_vertex[2].y = clientParams_p->height;
		clientParams_p->stuTrack_vertex[3].x = 0;
		clientParams_p->stuTrack_vertex[3].y = clientParams_p->height;
	}
	return CHECH_STURRACK_RESULT_OK;
}

static int initStat(Stu_Analysis_t * pStu_statistics)
{
	pStu_statistics->moveCount = 0;
	track_timerClear(&(pStu_statistics->standUpTimer));
	return 0;
}

#define RANGE_STUTRACK_DETECTION_ROISTART 15
itc_BOOL stuTrack_initializeTrack(StuITRACK_Params * inst, StuITRACK_InteriorParams* interior_params_p)
{
	if (inst == NULL || interior_params_p == NULL)
	{
		return FALSE;
	}
	stuTrack_stopTrack(inst, interior_params_p);

#ifndef _WIN32
	Vps_printf("inst->clientParams.flag_setting=%d\n", inst->clientParams.flag_setting);
	Vps_printf("inst->clientParams.height=%d\n", inst->clientParams.height);
	Vps_printf("inst->clientParams.width=%d\n", inst->clientParams.width);
	Vps_printf("inst->clientParams.stuTrack_debugMsg_flag=%d\n", inst->clientParams.stuTrack_debugMsg_flag);
	Vps_printf("inst->clientParams.stuTrack_Draw_flag=%d\n", inst->clientParams.stuTrack_Draw_flag);
	Vps_printf("inst->clientParams.stuTrack_direct_standard=%d,%d,%d,%d\n", inst->clientParams.stuTrack_direct_standard[0], inst->clientParams.stuTrack_direct_standard[1], inst->clientParams.stuTrack_direct_standard[2], inst->clientParams.stuTrack_direct_standard[3]);
	Vps_printf("inst->clientParams.stuTrack_stuWidth_standard=%d,%d,%d,%d\n", inst->clientParams.stuTrack_stuWidth_standard[0], inst->clientParams.stuTrack_stuWidth_standard[1], inst->clientParams.stuTrack_stuWidth_standard[2], inst->clientParams.stuTrack_stuWidth_standard[3]);
	Vps_printf("inst->clientParams.stuTrack_direct_range=%d\n", inst->clientParams.stuTrack_direct_range);
	Vps_printf("inst->clientParams.stuTrack_standCount_threshold=%d\n", inst->clientParams.stuTrack_standCount_threshold);
	Vps_printf("inst->clientParams.stuTrack_sitdownCount_threshold=%d\n", inst->clientParams.stuTrack_sitdownCount_threshold);
	Vps_printf("inst->clientParams.stuTrack_moveDelayed_threshold=%d\n", inst->clientParams.stuTrack_moveDelayed_threshold);
	Vps_printf("inst->clientParams.stuTrack_move_threshold=%lf\n", inst->clientParams.stuTrack_move_threshold);
	Vps_printf("inst->clientParams.stuTrack_standup_threshold=%lf\n", inst->clientParams.stuTrack_standup_threshold);
	Vps_printf("inst->clientParams.stretchingAB=%lf,%lf\n", inst->clientParams.stretchingAB[0], inst->clientParams.stretchingAB[1]);
#endif

	if (inst->systemParams.callbackmsg_func != NULL)
	{	interior_params_p->callbackmsg_func = inst->systemParams.callbackmsg_func;}
	else
	{	interior_params_p->callbackmsg_func = printf;}
	interior_params_p->srcimg_size.width = 0;
	interior_params_p->srcimg_size.height = 0;
	interior_params_p->initialize_flag = FALSE;

	double size_threshold_a, size_threshold_b;
	double direct_threshold_a, direct_threshold_b;
	if ((inst->clientParams.flag_setting == TRUE) && (stuTrack_check_clientParams(&inst->clientParams) == CHECH_STURRACK_RESULT_OK))
	{
		//��Ĭ�ϲ���
		interior_params_p->rangeStart = ITC_MIN(inst->clientParams.stuTrack_vertex[0].y, inst->clientParams.stuTrack_vertex[1].y) - RANGE_STUTRACK_DETECTION_ROISTART;
		interior_params_p->rangeStart = ITC_MAX(interior_params_p->rangeStart, 0);
		interior_params_p->rangeEnd = ITC_MAX(inst->clientParams.stuTrack_vertex[2].y, inst->clientParams.stuTrack_vertex[3].y);

		interior_params_p->rangeStartX = ITC_MIN(inst->clientParams.stuTrack_vertex[0].x, inst->clientParams.stuTrack_vertex[3].x);
		interior_params_p->rangeEndX = ITC_MAX(inst->clientParams.stuTrack_vertex[1].x, inst->clientParams.stuTrack_vertex[2].x);

		int y1 = (inst->clientParams.stuTrack_vertex[0].y + inst->clientParams.stuTrack_vertex[1].y) / 2;
		int y2 = (inst->clientParams.stuTrack_vertex[2].y + inst->clientParams.stuTrack_vertex[3].y) / 2;
		if (y1 == y2)
		{
			_PRINTF("The input parameter error:y1 == y2!\n");
			return FALSE;
		}
		y1 *= y1;
		y2 *= y2;
		int width1 = (inst->clientParams.stuTrack_stuWidth_standard[0] + inst->clientParams.stuTrack_stuWidth_standard[1]) / 2;
		int width2 = (inst->clientParams.stuTrack_stuWidth_standard[2] + inst->clientParams.stuTrack_stuWidth_standard[3]) / 2;
		size_threshold_a = ((double)(width1 - width2)) / (y1 - y2);
		size_threshold_b = width1 - size_threshold_a*y1;

		int x1 = (inst->clientParams.stuTrack_vertex[0].x + inst->clientParams.stuTrack_vertex[3].x) / 2;
		int x2 = (inst->clientParams.stuTrack_vertex[1].x + inst->clientParams.stuTrack_vertex[2].x) / 2;
		if (x1 == x2)
		{
			_PRINTF("The input parameter error:x1 == x2!\n");
			return FALSE;
		}
		int direct1 = (ITC_NORM_ANGLE360(inst->clientParams.stuTrack_direct_standard[0]) + ITC_NORM_ANGLE360(inst->clientParams.stuTrack_direct_standard[3])) / 2;
		int direct2 = (ITC_NORM_ANGLE360(inst->clientParams.stuTrack_direct_standard[1]) + ITC_NORM_ANGLE360(inst->clientParams.stuTrack_direct_standard[2])) / 2;
		direct_threshold_a = ((double)(direct1 - direct2)) / (x1 - x2);
		direct_threshold_b = direct1 - direct_threshold_a*x1;

		interior_params_p->img_size.width = (inst->clientParams.width + ITC_IMAGE_ALIGN - 1)&~(ITC_IMAGE_ALIGN - 1);		//���뵽8λ
		interior_params_p->img_size.height = (inst->clientParams.height + ITC_IMAGE_ALIGN - 1)&~(ITC_IMAGE_ALIGN - 1);
		interior_params_p->stuTrack_debugMsg_flag			= inst->clientParams.stuTrack_debugMsg_flag;
		interior_params_p->stuTrack_Draw_flag				= inst->clientParams.stuTrack_Draw_flag;
		interior_params_p->stuTrack_direct_range = inst->clientParams.stuTrack_direct_range;						//����ʱ����ĽǶ�ƫ�뷶Χ
		interior_params_p->stuTrack_standCount_threshold	= inst->clientParams.stuTrack_standCount_threshold;		//�ж�Ϊ������֡����ֵ
		interior_params_p->stuTrack_sitdownCount_threshold	= inst->clientParams.stuTrack_sitdownCount_threshold;	//�ж�Ϊ���µ�֡����ֵ
		interior_params_p->stuTrack_moveDelayed_threshold	= inst->clientParams.stuTrack_moveDelayed_threshold;	//�ƶ�Ŀ�걣�ָ��ٵ���ʱ���������ʱ�����˶������������(��λ������)
		interior_params_p->stuTrack_deleteTime_threshold = inst->clientParams.stuTrack_deleteTime_threshold * 1000;	//��ת��Ϊ����
		interior_params_p->stuTrack_move_threshold = inst->clientParams.stuTrack_move_threshold;			//�ж����ƶ�Ŀ���ƫ����ֵ������������ȵı�ֵ��
		interior_params_p->stuTrack_standup_threshold = inst->clientParams.stuTrack_standup_threshold;		//�ж�������Ŀ���ƫ����ֵ������������ȵı�ֵ��

		interior_params_p->transformationMatrix = itc_create_mat(3, 3, ITC_64FC1);
		interior_params_p->transformationMatrix->data.db[0] = inst->clientParams.transformationMatrix[0];
		interior_params_p->transformationMatrix->data.db[1] = inst->clientParams.transformationMatrix[1];
		interior_params_p->transformationMatrix->data.db[2] = inst->clientParams.transformationMatrix[2];
		interior_params_p->transformationMatrix->data.db[3] = inst->clientParams.transformationMatrix[3];
		interior_params_p->transformationMatrix->data.db[4] = inst->clientParams.transformationMatrix[4];
		interior_params_p->transformationMatrix->data.db[5] = inst->clientParams.transformationMatrix[5];
		interior_params_p->transformationMatrix->data.db[6] = inst->clientParams.transformationMatrix[6];
		interior_params_p->transformationMatrix->data.db[7] = inst->clientParams.transformationMatrix[7];
		interior_params_p->transformationMatrix->data.db[8] = inst->clientParams.transformationMatrix[8];

		interior_params_p->stretchingAB[0] = inst->clientParams.stretchingAB[0];
		interior_params_p->stretchingAB[1] = inst->clientParams.stretchingAB[1];
	}
	else
	{
		//Ĭ�ϵĲ���
		interior_params_p->img_size.width = WIDTH_STUTRACK_IMG_;
		interior_params_p->img_size.height = HEIGHT_STUTRACK_IMG_;
		interior_params_p->stuTrack_debugMsg_flag			= 1;
		interior_params_p->stuTrack_Draw_flag				= TRUE;

		interior_params_p->stuTrack_direct_range = RANGE_STUTRACK_STANDDIRECT_DEFALUT_PARAMS;					//����ʱ����ĽǶ�ƫ�뷶Χ
		interior_params_p->stuTrack_standCount_threshold	= THRESHOLD_STUTRACK_STANDCOUNT_DEFALUT_PARAMS;		//�ж�Ϊ������֡����ֵ
		interior_params_p->stuTrack_sitdownCount_threshold	= THRESHOLD_STUTRACK_SITDOWNCOUNT_DEFALUT_PARAMS;	//�ж�Ϊ���µ�֡����ֵ
		interior_params_p->stuTrack_moveDelayed_threshold	= THRESHOLD_STUTRACK_MOVEDELAYED_DEFALUT_TIME;		//�ƶ�Ŀ�걣�ָ��ٵ���ʱ���������ʱ�����˶������������(��λ������)
		interior_params_p->stuTrack_deleteTime_threshold	= THRESHOLD_STURECK_ALL_DELETE_TIME;				//Ŀ�걣�ָ��ٵ��ʱ��ʱ���������ʱ��ɾ��Ŀ��(��λ������)
		interior_params_p->stuTrack_move_threshold = THRESHOLD_STUTRACK_MOVE_DEFALUT_PARAMS;			//�ж����ƶ�Ŀ���ƫ����ֵ������������ȵı�ֵ��
		interior_params_p->stuTrack_standup_threshold = THRESHOLD_STUTRACK_STANDUP_DEFALUT_PARAMS;		//�ж�������Ŀ���ƫ����ֵ����ֵ��

		interior_params_p->rangeStart = 0;
		interior_params_p->rangeEnd = HEIGHT_STUTRACK_IMG_;
		interior_params_p->rangeStartX = 0;
		interior_params_p->rangeEndX = WIDTH_STUTRACK_IMG_;

		size_threshold_a = A_STUTRACK_SIZE_THRESHOLD_PARAMS;
		size_threshold_b = B_STUTRACK_SIZE_THRESHOLD_PARAMS;
		direct_threshold_a = A_STUTRACK_DIRECT_THRESHOLD_PARAMS;
		direct_threshold_b = B_STUTRACK_DIRECT_THRESHOLD_PARAMS;

		interior_params_p->transformationMatrix = itc_create_mat(3, 3, ITC_64FC1);
		MATRIX_STUTRACK_DEFAULT_PARAMS;
		interior_params_p->transformationMatrix->data.db[0] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(0);
		interior_params_p->transformationMatrix->data.db[1] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(1);
		interior_params_p->transformationMatrix->data.db[2] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(2);
		interior_params_p->transformationMatrix->data.db[3] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(3);
		interior_params_p->transformationMatrix->data.db[4] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(4);
		interior_params_p->transformationMatrix->data.db[5] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(5);
		interior_params_p->transformationMatrix->data.db[6] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(6);
		interior_params_p->transformationMatrix->data.db[7] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(7);
		interior_params_p->transformationMatrix->data.db[8] = MATRIX_STUTRACK_DEFAULT_PARAMS_AT(8);
		interior_params_p->stretchingAB[0] = STRETCHING_STUTRACK_DEFFAULT_PARAMS_A;
		interior_params_p->stretchingAB[1] = STRETCHING_STUTRACK_DEFFAULT_PARAMS_B;
	}

	//��ʼ�����е��ڲ�ͳ�Ʋ���
	interior_params_p->_count = 0;
	interior_params_p->count_stuTrack_rect = 0;
	interior_params_p->count_trackObj_allState = 0;
	
	interior_params_p->last_upcast_time = gettime();
	interior_params_p->move_camera_time = gettime();
	interior_params_p->old_move_Stopflag = FALSE;
	interior_params_p->OldResult_flag = RESULT_STUTRAKC_NULL_CAMERA;
	interior_params_p->move_csucceed_flag = FALSE;
	
	interior_params_p->old_move_position.x = 0;			//Ŀ��λ��
	interior_params_p->old_move_position.y = 0;			
	interior_params_p->old_move_stretchingCoefficient = 0;			//����ϵ��

	//�����ڴ�
	interior_params_p->currMat = itc_create_mat(interior_params_p->img_size.height, interior_params_p->img_size.width, ITC_8UC1);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->currMat, FALSE);

	interior_params_p->lastMat = itc_create_mat(interior_params_p->img_size.height, interior_params_p->img_size.width, ITC_8UC1);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->lastMat, FALSE);

	interior_params_p->mhiMat = itc_create_mat(interior_params_p->img_size.height, interior_params_p->img_size.width, ITC_8UC1);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->mhiMat, FALSE);

	interior_params_p->maskMat = itc_create_mat(interior_params_p->img_size.height, interior_params_p->img_size.width, ITC_8UC1);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->maskMat, FALSE);

	interior_params_p->current_BKid = -1;
	int i = 0;
	for (i = 0; i < SATUTRACK_BK_IMG_COUNT;i++)
	{
		interior_params_p->BKmat[i] = itc_create_mat(interior_params_p->img_size.height, interior_params_p->img_size.width, ITC_8UC1);
		JUDEGE_STUREACK_IF_NULL(interior_params_p->BKmat[i], FALSE);
	}

	interior_params_p->stuTrack_storage = itcCreateMemStorage(0);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->stuTrack_storage, FALSE);

	interior_params_p->stuTrack_rect_arr = (Track_Rect_t*)itcAlloc(sizeof(Track_Rect_t)* COUNT_STUTRACK_MALLOC_ELEMENT);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->stuTrack_rect_arr, FALSE);

	interior_params_p->stuTrack_allState = (StuTrack_allState_t*)itcAlloc(sizeof(StuTrack_allState_t)* COUNT_STUTRACK_MALLOC_ELEMENT);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->stuTrack_allState, FALSE);

	interior_params_p->stuTrack_size_threshold = (int *)itcAlloc(sizeof(int)* interior_params_p->img_size.height);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->stuTrack_size_threshold, FALSE);

	interior_params_p->stuTrack_direct_threshold = (int *)itcAlloc(sizeof(int)* interior_params_p->img_size.width);
	JUDEGE_STUREACK_IF_NULL(interior_params_p->stuTrack_direct_threshold, FALSE);

	memset(interior_params_p->stuTrack_rect_arr, 0, sizeof(Track_Rect_t)* COUNT_STUTRACK_MALLOC_ELEMENT);
	memset(interior_params_p->stuTrack_allState, 0, sizeof(StuTrack_allState_t)* COUNT_STUTRACK_MALLOC_ELEMENT);
	memset(interior_params_p->stuTrack_size_threshold, 0, sizeof(int)* interior_params_p->img_size.height);
	memset(interior_params_p->stuTrack_direct_threshold, 0, sizeof(int)* interior_params_p->img_size.width);

	int x = 0;
	for (x = 0; x < interior_params_p->img_size.width; x++)
	{
		//��ʼ����׼�����Ƕ�
		interior_params_p->stuTrack_direct_threshold[x] = (int)(COMPUTER_STUTRACK_DIRECT_THRESHOLD_PARAMS(x, direct_threshold_a, direct_threshold_b) + 0.5);
	}

	int y = 0;
	int size_stara = (int)(COMPUTER_STUTRACK_SIZE_THRESHOLD_PARAMS(interior_params_p->rangeStart, size_threshold_a, size_threshold_b) + 0.5);
	int size_end = (int)(COMPUTER_STUTRACK_SIZE_THRESHOLD_PARAMS(interior_params_p->rangeEnd, size_threshold_a, size_threshold_b) + 0.5);
	for (y = 0; y < interior_params_p->rangeStart; y++)
	{
		interior_params_p->stuTrack_size_threshold[y] = size_stara;
	}
	for (y = interior_params_p->rangeStart; y < interior_params_p->rangeEnd; y++)
	{
		//��ʼ��ɸѡ�ߴ���ֵ
		interior_params_p->stuTrack_size_threshold[y] = (int)(COMPUTER_STUTRACK_SIZE_THRESHOLD_PARAMS(y, size_threshold_a, size_threshold_b) + 0.5);
	}
	for (y = interior_params_p->rangeEnd; y < interior_params_p->img_size.height; y++)
	{
		interior_params_p->stuTrack_size_threshold[y] = size_end;
	}

	//��ʼ��������ɫ
	interior_params_p->pink_colour = colour_RGB2YUV(255, 0, 255);	/*�ۺ�*/
	interior_params_p->blue_colour = colour_RGB2YUV(0, 0, 255);		/*����*/
	interior_params_p->lilac_colour = colour_RGB2YUV(155, 155, 255);/*����*/
	interior_params_p->green_colour = colour_RGB2YUV(0, 255, 0);	/*����*/
	interior_params_p->red_colour = colour_RGB2YUV(255, 0, 0);		/*����*/
	interior_params_p->dullred_colour = colour_RGB2YUV(127, 0, 0);	/*����*/
	interior_params_p->yellow_colour = colour_RGB2YUV(255, 255, 0);	/*����*/

	interior_params_p->bStat_flag = FALSE;

	interior_params_p->initialize_flag = TRUE;
	return TRUE;
}

#define THRESHOLD_STUTRACK_FRAME_DIFF	13
#define THERSHOLD_STUTRAKC_HMI_MASK		240
stuTrackReturn stuTrack_process(const StuITRACK_Params *inst, StuITRACK_InteriorParams* interior_params_p, StuITRACK_OutParams_t* return_params, char* imageData, char* bufferuv)
{
	if (imageData == NULL || return_params == NULL || interior_params_p == NULL || inst == NULL )
	{
		return RETURN_STUTRACK_noNEED_PROCESS;
	}
	else if (interior_params_p->initialize_flag == FALSE)
	{
		_PRINTF("Failed to initialize!\n");
		return RETURN_STUTRACK_noNEED_PROCESS;
	}
	ITC_FUNCNAME("FUNCNAME:stuTrack_process\n");
	if (interior_params_p->srcimg_size.width == 0 || interior_params_p->srcimg_size.height == 0)																									 
	{																																																 
		if (inst->systemParams.nsrcWidth == 0 || inst->systemParams.nsrcHeight == 0)																												 
		{																																															 
			_PRINTF("The original image size error!\n");																																			 
			return RETURN_STUTRACK_noNEED_PROCESS;
		}																																															 
		interior_params_p->srcimg_size.width = inst->systemParams.nsrcWidth;																														 
		interior_params_p->srcimg_size.height = inst->systemParams.nsrcHeight;									
	}

	stuTrack_Copy_matData(interior_params_p, (itc_uchar*)imageData);
	interior_params_p->result_flag = RESULT_STUTRACK_NULL_FLAG;	//��ձ仯״̬
	Track_Contour_t* firstContour = NULL;
	if ((interior_params_p->_count&SATUTRACK_BK_IMG_FRAME_INTERVAL) == 0)
	{
		interior_params_p->current_BKid++;
		if (interior_params_p->current_BKid >= SATUTRACK_BK_IMG_COUNT)
		{
			interior_params_p->current_BKid -= SATUTRACK_BK_IMG_COUNT;
		}
		track_copyImage(interior_params_p->currMat, interior_params_p->BKmat[interior_params_p->current_BKid]);
	}
	if (interior_params_p->_count>1)
	{
		itcClearMemStorage(interior_params_p->stuTrack_storage);
		track_update_MHI(interior_params_p->currMat, interior_params_p->lastMat, interior_params_p->mhiMat, THRESHOLD_STUTRACK_FRAME_DIFF, interior_params_p->maskMat, THERSHOLD_STUTRAKC_HMI_MASK);
		track_find_contours(interior_params_p->maskMat, &firstContour, interior_params_p->stuTrack_storage);
		stuTrack_filtrate_contours(interior_params_p,&firstContour);
		stuTrack_proStandDown_ROI(interior_params_p);
	}

	ITC_SWAP(interior_params_p->currMat, interior_params_p->lastMat, interior_params_p->tempMat);
	interior_params_p->_count++;

	stuTrackReturn flag=stuTrack_reslut(interior_params_p, return_params);//��д���ٽ��
	stuTrack_drawShow_imgData(interior_params_p, return_params, (itc_uchar*)imageData, (itc_uchar*)bufferuv);	//���ƴ���Ч��

	return flag;//���ؽ��									
}

void stuTrack_stopTrack(const StuITRACK_Params *inst, StuITRACK_InteriorParams* interior_params_p)
{
	if (inst == NULL || interior_params_p == NULL)
	{
		return;
	}
	ITC_FUNCNAME("FUNCNAME:stuTrack_stopTrack\n");

	if (interior_params_p->stuTrack_size_threshold != NULL)
	{
		itcFree_(interior_params_p->stuTrack_size_threshold);
		interior_params_p->stuTrack_size_threshold = NULL;
	}

	if (interior_params_p->stuTrack_direct_threshold != NULL)
	{
		itcFree_(interior_params_p->stuTrack_direct_threshold);
		interior_params_p->stuTrack_direct_threshold = NULL;
	}

	if (interior_params_p->stuTrack_rect_arr != NULL)
	{
		itcFree_(interior_params_p->stuTrack_rect_arr);
		interior_params_p->stuTrack_rect_arr = NULL;
	}

	if (interior_params_p->stuTrack_allState != NULL)
	{
		itcFree_(interior_params_p->stuTrack_allState);
		interior_params_p->stuTrack_allState = NULL;
	}

	interior_params_p->count_stuTrack_rect = 0;
	interior_params_p->count_trackObj_allState = 0;

	
	itcReleaseMemStorage(&interior_params_p->stuTrack_storage);
	itc_release_mat(&interior_params_p->transformationMatrix);
	itc_release_mat(&interior_params_p->currMat);
	itc_release_mat(&interior_params_p->lastMat);
	itc_release_mat(&interior_params_p->mhiMat);
	itc_release_mat(&interior_params_p->maskMat);
	int i = 0;
	for (i = 0; i < SATUTRACK_BK_IMG_COUNT; i++)
	{
		itc_release_mat(&(interior_params_p->BKmat[i]));
	}

	initStat(&(interior_params_p->stu_statistics));

	interior_params_p->initialize_flag = FALSE;
}

void stuTrack_statisticsSwitch(StuITRACK_InteriorParams* interior_params_p, int flag)
{
	if (interior_params_p->initialize_flag == TRUE)
	{
		if (flag == TRUE)
		{
			initStat(&(interior_params_p->stu_statistics));
		}
		interior_params_p->bStat_flag = flag;
	}
}

Stu_Analysis_t* stuTrack_statisticsGet(StuITRACK_InteriorParams* interior_params_p)
{
	if (interior_params_p->initialize_flag == TRUE)
	{
		return &interior_params_p->stu_statistics;
	}
	else
	{
		return NULL;
	}
}
