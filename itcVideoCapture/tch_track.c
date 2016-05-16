#include "tch_track.h"

#define PRESET_ALIGN 10
//list operation type
#define ADD 1
#define REMOVE 0
//timer operation type
#define UPDATE 0
#define RESET 1
#define CLEAR 2

static void tchTrack_Copy_matData(Tch_Data_t* datas, itc_uchar* srcData);
static void tchTrack_drawShow_imgData(Tch_Data_t* interior_params, itc_uchar* imageData, itc_uchar* bufferuv, Track_Rect_t *rect, Track_Colour_t *color);
static int tch_return_maintain(Tch_Timer_t *tch_timer, int in);
static int tch_isBlackBoard(int numBlk, int numTch, Track_Rect_t *rectBlk, Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params, itc_uchar *src, itc_uchar* pUV);
static int tch_singleTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static int tch_multipleTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static int tch_noneTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static void tch_updatePosition(Tch_Data_t *data);
static void tch_updateFeatureRect(Tch_Data_t *data);
static void tch_updateTargetRcd(Tch_Data_t *data, int index, int type);

int tch_destroyAnalysis(Tch_Analysis_t **analysis)
{
	if (*analysis==NULL)
	{
		return -1;
	}
	track_timerDestroy(&(*analysis)->outTimer, &(*analysis)->cntOutside);
	track_timerDestroy(&(*analysis)->mlpTimer, &(*analysis)->cntMultiple);
	free(*analysis);
	*analysis = NULL;
	return 0;
}

int tch_switchAnalysis(Tch_Data_t *data)
{
	if (data==NULL)
	{
		return -1;
	}
	if (data->isAnalysing==1)
	{
		data->isAnalysing = 0;
		tch_destroyAnalysis(&data->analysis);
		return data->isAnalysing;
	}
	else
	{
		tch_destroyAnalysis(&data->analysis);
		data->isAnalysing = 1;
		data->analysis = track_analysisCreate(TEACHER);
		data->nodeOutside = track_timerInit();
		data->nodeMultiple = track_timerInit();
		return data->isAnalysing;
	}
}

static void tchTrack_Copy_matData(Tch_Data_t* datas, itc_uchar* srcData)
{
	//ITC_FUNCNAME("FUNCNAME:stuTrack_resizeCopy_matData\n");
	int y = 0;
	int tch_step = datas->g_anaWin.width;
	int blk_step = datas->g_blkWin.width;
	int src_step = datas->sysData.width;
	if (tch_step > src_step)
	{
		//_PRINTF("The image cache size error!\n");
		datas->sysData.callbackmsg_func("The image cache size error!\n");
		return;
	}

	if (blk_step > src_step)
	{
		//_PRINTF("The image cache size error!\n");
		datas->sysData.callbackmsg_func("The image cache size error!\n");
		return;
	}

	itc_uchar* dst_blk = datas->currMatBlk->data.ptr;
	itc_uchar* src_blk = srcData + src_step*datas->g_blkWin.y + datas->g_blkWin.x;
	for (y = 0; y < datas->g_blkWin.height; y++)
	{
		memcpy(dst_blk, src_blk, sizeof(itc_uchar)* blk_step);
		dst_blk += blk_step;
		src_blk += src_step;
	}

	itc_uchar* dst_tch = datas->currMatTch->data.ptr;
	itc_uchar* src_tch = srcData + src_step*datas->g_anaWin.y + datas->g_anaWin.x;
	for (y = 0; y < datas->g_anaWin.height; y++)
	{
		memcpy(dst_tch, src_tch, sizeof(itc_uchar)* tch_step);
		dst_tch += tch_step;
		src_tch += src_step;
	}
}

int tch_trackInit(Tch_Data_t *data)
{
	if (!data)
	{
		//data->sysData.callbackmsg_func("Tch_Data_t err.");
		return -1;
	}

	if (data->numOfPos == 0)
	{
		data->sysData.callbackmsg_func("Preset number err.");
		return -1;
	}
	//data->track_pos_width = data->g_frameSize.width / data->numOfPos;
	data->track_pos_width = data->g_tchWin.width / data->numOfPos;

	data->tch_lastStatus = RETURN_TRACK_TCH_NULL;

	
	data->g_posIndex = -1;
	data->g_prevPosIndex = -1;
	//tch_pos = &g_posIndex;

	data->g_isMulti = 0;
	data->g_isOnStage = 0;
	data->g_count = 0;

	data->lastRectNum = 0;

	data->slideTimer.start = 0;
	data->slideTimer.finish = 0;
	data->slideTimer.deltaTime = 0;

	data->tch_timer.start = 0;
	data->tch_timer.finish = 0;
	data->tch_timer.deltaTime = 0;

	data->outsideTimer.start = 0;
	data->outsideTimer.finish = 0;
	data->outsideTimer.deltaTime = 0;

	data->pos_slide.width = (data->numOfSlide - 1) / 2;
	data->pos_slide.center = -1;
	data->pos_slide.left = -1;
	data->pos_slide.right = -1;

	data->g_anaWin.x = 0;
	data->g_anaWin.y = data->g_tchWin.y;
	data->g_anaWin.width = data->g_frameSize.width;
	data->g_anaWin.height = data->g_tchWin.height;


	data->cam_pos = calloc(data->numOfPos, sizeof(Tch_CamPosition_t));
	Tch_CamPosition_t *ptr = data->cam_pos;

	memset(data->g_lastTarget, 0, MAX_TARGET*sizeof(Track_Rect_t));

	int cnt = 0;
	int i = 0;
	while (cnt<data->numOfPos)
	{
		ptr->index = i / data->track_pos_width;
		ptr->left_pixel = i + data->g_tchWin.x;
		ptr->right_pixel = i + data->g_tchWin.x + data->track_pos_width;
		i += data->track_pos_width;
		ptr++;
		cnt++;
	}
	
	ptr = NULL;

	//data->srcMat = itc_create_mat(data->g_frameSize.height, data->g_frameSize.width, ITC_8UC1);

	data->prevMatTch = itc_create_mat(data->g_anaWin.height, data->g_anaWin.width, ITC_8UC1);
	data->prevMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->tempMatTch = NULL;
	data->tempMatBlk = NULL;

	data->currMatTch = itc_create_mat(data->g_anaWin.height, data->g_anaWin.width, ITC_8UC1);
	data->mhiMatTch = itc_create_mat(data->g_anaWin.height, data->g_anaWin.width, ITC_8UC1);
	data->maskMatTch = itc_create_mat(data->g_anaWin.height, data->g_anaWin.width, ITC_8UC1);
	data->currMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->mhiMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->maskMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);

	data->storage = itcCreateMemStorage(0);
	data->storageTch = itcCreateChildMemStorage(data->storage);
	data->storageBlk = itcCreateChildMemStorage(data->storage);

	/*data->isAnalysing = 1;
	data->analysis = track_analysisCreate();
	data->nodeOutside = track_timerInit();
	data->nodeMultiple = track_timerInit();*/

	return 0;
}


static void tchTrack_drawShow_imgData(Tch_Data_t* interior_params, itc_uchar* imageData, itc_uchar* bufferuv, Track_Rect_t *rect, Track_Colour_t *color)
{
	Track_Size_t srcimg_size = { interior_params->sysData.width, interior_params ->sysData.height};	//原始图像大小
#ifdef _WIN32
	int YUV420_type = TRACK_DRAW_YUV420P;
#endif
#ifndef _WIN32
	int YUV420_type = TRACK_DRAW_YUV420SP;
#endif
	track_draw_rectangle(imageData, bufferuv, &srcimg_size, rect, color, YUV420_type);
}


static void tch_updateTimer(Tch_Timer_t *timer,int type)
{
	if (type==UPDATE)
	{
		timer->finish = gettime();
		timer->deltaTime += timer->finish - timer->start;
		timer->start = timer->finish;
		return;
	}
	if (type == RESET)
	{
		timer->finish = 0;
		timer->deltaTime = 0;
		timer->start = gettime();
		return;
	}
	if (type == CLEAR)
	{
		timer->finish = 0;
		timer->deltaTime = 0;
		timer->start = 0;
		return;
	}
}

#define TRACK_MAINTAIN_TIME 2000//维护返回值的时间，2秒钟返回一个结果
static int tch_return_maintain(Tch_Timer_t *tch_timer, int in)
{
	int out;
	if (in == TRUE)
	{
		tch_timer->deltaTime = 0;
		tch_timer->start = gettime();
		out = TRUE;
		return out;
	}
	else
	{
		tch_timer->finish = gettime();
		tch_timer->deltaTime = tch_timer->finish - tch_timer->start;
		if (tch_timer->deltaTime>TRACK_MAINTAIN_TIME)
		{
			tch_timer->deltaTime = 0;
			tch_timer->start = gettime();
			out = TRUE;
		}
		else
		{
			out = FALSE;
		}
		return out;
	}
}

static void tch_updateFeatureRect(Tch_Data_t *data)
{
	if (data->lastRectNum>1)//如果目标数目大于1，只更新时间
	{
		int i;
		for (i = 0; i < data->lastRectNum; i++)
		{
			tch_updateTimer(&data->g_lastTarget[i].timer,UPDATE);
		}
		//TCH_PRINTF("updating multiple time!!!!!\r\n");
		return;
	}
	//更新位置
	data->g_prevPosIndex = data->g_posIndex;
	//pos_1 = data->center;
	if (data->g_posIndex == data->pos_slide.right || data->g_posIndex == data->pos_slide.left)//当人物走到滑框的边缘，就更新滑框，相当于调全景移动云台相机，修改为走到边缘前一个预置位就更新镜头
	{
		if (data->g_posIndex < data->pos_slide.width)//左端
		{
			if (data->g_posIndex == 0)//在最左端时维护时间
			{
				/*data->slideTimer.finish = gettime();
				data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
				data->slideTimer.start = data->slideTimer.finish;*/
				//TCH_PRINTF("updating time!!!!!\r\n");
				tch_updateTimer(&data->g_lastTarget[0].timer,UPDATE);
			}
			else//否则更新时间
			{
				/*data->slideTimer.finish = 0;
				data->slideTimer.deltaTime = 0;
				data->slideTimer.start = gettime();*/
				//TCH_PRINTF("reseting time!!!!!\r\n");
				if (data->isAnalysing)
				{
					if (data->analysis->standTimer.start != 0)
					{
						track_timerEnd(&data->analysis->standTimer);
					}
					if (data->analysis->moveTimer.start == 0)
					{
						track_timerStart(&data->analysis->moveTimer);//分析
					}
				}
				tch_updateTimer(&data->g_lastTarget[0].timer, RESET);
			}
			data->pos_slide.center = data->pos_slide.width;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		else if (data->g_posIndex > data->numOfPos - 1 - data->pos_slide.width)//右端
		{
			if (data->g_posIndex == data->numOfPos - 1)//在最右端时维护时间
			{
				/*data->slideTimer.finish = gettime();
				data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
				data->slideTimer.start = data->slideTimer.finish;*/
				//TCH_PRINTF("updating time!!!!!\r\n");
				tch_updateTimer(&data->g_lastTarget[0].timer, UPDATE);
			}
			else//否则更新时间
			{
				/*data->slideTimer.finish = 0;
				data->slideTimer.deltaTime = 0;
				data->slideTimer.start = gettime();*/
				//TCH_PRINTF("reseting time!!!!!\r\n");
				if (data->isAnalysing)
				{
					if (data->analysis->standTimer.start != 0)
					{
						track_timerEnd(&data->analysis->standTimer);
					}
					if (data->analysis->moveTimer.start == 0)
					{
						track_timerStart(&data->analysis->moveTimer);//分析
					}
				}
				
				tch_updateTimer(&data->g_lastTarget[0].timer, RESET);
			}
			data->pos_slide.center = data->numOfPos - 1 - data->pos_slide.width;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		else
		{
			data->pos_slide.center = data->g_posIndex;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;

			/*data->slideTimer.finish = 0;
			data->slideTimer.deltaTime = 0;
			data->slideTimer.start = gettime();*/
			//TCH_PRINTF("reseting time!!!!!\r\n");
			if (data->isAnalysing)
			{
				if (data->analysis->standTimer.start != 0)
				{
					track_timerEnd(&data->analysis->standTimer);
				}
				if (data->analysis->moveTimer.start == 0)
				{
					track_timerStart(&data->analysis->moveTimer);//分析
				}
			}
			tch_updateTimer(&data->g_lastTarget[0].timer, RESET);
		}
	}
	else if (data->pos_slide.left > data->g_posIndex || data->g_posIndex > data->pos_slide.right)
	{
		//当追踪到的目标已经在上一帧滑框的外面则立刻更新滑框
		if (data->g_posIndex < data->pos_slide.width)
		{
			data->pos_slide.center = data->pos_slide.width;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		else if (data->g_posIndex > data->numOfPos - 1 - data->pos_slide.width)
		{
			data->pos_slide.center = data->numOfPos - 1 - data->pos_slide.width;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		else
		{
			data->pos_slide.center = data->g_posIndex;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		/*data->slideTimer.finish = 0;
		data->slideTimer.deltaTime = 0;
		data->slideTimer.start = gettime();*/
		//TCH_PRINTF("reseting time!!!!!\r\n");
		if (data->isAnalysing)
		{
			if (data->nodeOutside->start != 0 && data->nodeOutside->start != (ULINT)-1)
			{
				track_timerEnd(data->nodeOutside);
				/*if (data->analysis->cntOutside>=2)
				{
				track_timerDestroy(&data->analysis->outTimer, &data->analysis->cntOutside);
				}*/
				data->analysis->outTimer = track_timerIncrease(data->analysis->outTimer, &data->analysis->cntOutside, data->nodeOutside);
				memset(data->nodeOutside, -1, sizeof(Analysis_Timer_t));
				data->nodeOutside->deltatime = 0;
			}
			if (data->analysis->standTimer.start != 0)
			{
				track_timerEnd(&data->analysis->standTimer);
			}
			if (data->analysis->moveTimer.start == 0)
			{
				track_timerStart(&data->analysis->moveTimer);//分析
			}
		}
		

		tch_updateTimer(&data->g_lastTarget[0].timer, RESET);
	}
	else//在特写镜头范围内则维护时间
	{
		/*data->slideTimer.finish = gettime();
		data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
		data->slideTimer.start = data->slideTimer.finish;*/
		//TCH_PRINTF("updating time!!!!!\r\n");
		tch_updateTimer(&data->g_lastTarget[0].timer, UPDATE);
	}
}

static int tch_isBlackBoard(int numBlk, int numTch, Track_Rect_t *rectBlk, Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params,  itc_uchar *src, itc_uchar* pUV)
{
	if (data==NULL)
	{
		TCH_PRINTF("Data Error!\r\n");
		return TRACK_FALSE;
	}
	int i = 0;
	if (numBlk > 0)
	{
		Track_Rect_t drawRect;
		for (i = 0; i < numBlk; i++)
		{
			drawRect.x = rectBlk[i].x + data->g_blkWin.x;
			drawRect.y = rectBlk[i].y + data->g_blkWin.y;
			drawRect.width = rectBlk[i].width;
			drawRect.height = rectBlk[i].height;
			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->green_colour);
		}
		if (1 == numTch && 1==data->lastRectNum)
		{
			//获取运动方向
			int direct = -1;
			direct = tch_calculateDirect_TCH(data->mhiMatTch, rectTch[0]);
			data->g_lastTarget[0].rect = rectTch[0];
			if (direct > -1)
			{
				tch_updatePosition(data);
				drawRect.x = rectTch[0].x + data->g_anaWin.x;
				drawRect.y = rectTch[0].y + data->g_anaWin.y;
				drawRect.width = rectTch[0].width;
				drawRect.height = rectTch[0].height;
				//tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
				tch_updateFeatureRect(data);
			}
		}
		return TRACK_TRUE;
	}
	else
		return TRACK_FALSE;
}

#define OUTSIDE_THRSHLD 0 //outside threshold
static int tch_analyzeOutside(Tch_Data_t *data, TeaITRACK_Params *params)
{
	int i;
	int left = data->g_tchWin.x;
	int right = data->g_tchWin.x + data->g_tchWin.width;
	int cnt = 0;
	for (i = 0; i < data->lastRectNum; i++)
	{
		//TCH_PRINTF("No.%d y = %d\r\n", i, data->g_lastTarget[i].rect.y);
		if (data->g_lastTarget[i].rect.y>params->threshold.outside || 
			data->g_lastTarget[i].rect.x>right || 
			data->g_lastTarget[i].rect.x + data->g_lastTarget[i].rect.width<left)
		{
			//tch_updateTargetRcd(data, i, REMOVE);
			
			//if (data->g_lastTarget[i].start!=1)
			//{
			//	data->g_lastTarget[i].end++;
			//	if (data->g_lastTarget[i].end > OUTSIDE_THRSHLD)
			//	{
			//		//TCH_PRINTF("Remove No.%d!\r\n", i);
			//		tch_updateTargetRcd(data, i, REMOVE);
			//		cnt++;
			//	}
			//}

			data->g_lastTarget[i].end++;
			if (data->g_lastTarget[i].end > OUTSIDE_THRSHLD)
			{
				//TCH_PRINTF("Remove No.%d!\r\n", i);
				tch_updateTargetRcd(data, i, REMOVE);
				//if (data->lastRectNum==0)
				//{
				//	if (data->nodeOutside->start == (ULINT)-1)
				//	{
				//		track_timerStart(data->nodeOutside);//分析
				//	}
				//}
				cnt++;
			}
		}
		else
		{
			data->g_lastTarget[i].end = 0;
		}
		//if (data->g_lastTarget[i].rect.x>data->g_tchWin.x+data->g_tchWin.width || data->g_lastTarget[i].rect.x + data->g_lastTarget[i].rect.width<data->g_tchWin.x)
		//{
		//	data->g_lastTarget[i].end++;
		//	if (data->g_lastTarget[i].end > OUTSIDE_THRSHLD)
		//	{
		//		//TCH_PRINTF("Remove No.%d!\r\n", i);
		//		tch_updateTargetRcd(data, i, REMOVE);
		//		cnt++;
		//	}
		//}
		//else
		//{
		//	data->g_lastTarget[i].end = 0;
		//}
	}
	return cnt;
}

static void tch_mergeRects(Tch_Data_t *data)
{
	if (data->lastRectNum==1)
	{
		return;
	}
	int i, j;
	int flag = 0;
	int maxArea=0, index;
	Track_Rect_t temp;
	for (i = 0; i < data->lastRectNum;i++)
	{
		Track_Rect_t maxRect;
		
		for (j = 0; j < data->lastRectNum; j++)
		{
			if (j != i)
			{
				temp = data->g_lastTarget[i].rect;
				flag = track_intersect_rect2(&temp, &data->g_lastTarget[j].rect, -10);
				if (flag>maxArea)
				{
					maxRect = temp;
					maxArea = flag;
					index = j;
				}
			}
		}
		if (maxArea > 0)
		{
			data->g_lastTarget[i].rect = maxRect;
			tch_updateTargetRcd(data, index, REMOVE);
			i--;
			maxArea = 0;
		}
	}

	/*for (i = 0; i < data->lastRectNum; i++)
	{
		Track_Rect_t maxRect;
		for (j = 0; j < data->lastRectNum; j++)
		{
			if (j!=i)
			{
				temp = data->g_lastTarget[i].rect;
				flag = tch_intersect_rect(&temp, &data->g_lastTarget[j].rect, -10);
				if (flag>maxArea)
				{
					if (data->g_lastTarget[i].o_start == data->g_lastTarget[j].o_start&&data->g_lastTarget[i].o_x == data->g_lastTarget[j].o_x&&data->g_lastTarget[i].o_width == data->g_lastTarget[j].o_width)
					{
						TCH_PRINTF("Merge %d and %d!\r\n",i,j);
						maxRect = temp;
						maxArea = flag;
						index = j;
					}
					else
					{
						TCH_PRINTF("No merge!\r\n");
					}
				}
			}
		}
		if (maxArea > 0)
		{
			data->g_lastTarget[i].rect = maxRect;
			tch_updateTargetRcd(data, index, REMOVE);
			i--;
			maxArea = 0;
		}
	}*/
}

static int tch_matchRects(Track_Rect_t *current, int num, Tch_Data_t *data, TeaITRACK_Params *params)
{
	if (data==NULL)
	{
		return TRACK_FALSE;
	}
	int i, j, flag = 0;
	int index, maxArea = 0;
	if (data->lastRectNum==0)//当前没有目标，加入新目标
	{
		for (j = 0; j < num;j++)
		{
			//TCH_PRINTF("add multiple!\r\n");
			data->g_lastTarget[data->lastRectNum].rect = current[j];
			tch_updateTargetRcd(data, -1, ADD);
		}
		tch_mergeRects(data);
		//TCH_PRINTF("==============\r\n");
		return TRACK_TRUE;
	}
	else
	{
		
		for (j = 0; j < num; j++)
		{
			Track_Rect_t temp;
			//TCH_PRINTF("Current:%d: x = %d{\r\n", j, current[j].x);
			for (i = 0; i < data->lastRectNum; i++)
			{
				temp = current[j];
				flag = track_intersect_rect2(&temp, &data->g_lastTarget[i].rect, -10);
				//TCH_PRINTF("No.%d: x = %d\r\n", i, data->g_lastTarget[i]);
				if (flag>maxArea)
				{
					maxArea = flag;
					index = i;
				}
			}
			//TCH_PRINTF("}");
			if (maxArea>0)
			{
				//TCH_PRINTF("match No.%d!\r\n",index);
				/*if (current[j].y<params->threshold.outside&&data->g_lastTarget[index].start==1)
				{
					data->g_lastTarget[index].start = 0;
				}*/
				data->g_lastTarget[index].rect = current[j];
				tch_updateTimer(&data->g_lastTarget[i].timer, RESET);
				maxArea = 0;
			}
			else
			{
				if (data->lastRectNum < MAX_TARGET)
				{
					/*if (current[j].y>params->threshold.outside)
					{
						TCH_PRINTF("add new!\r\n");
						data->g_lastTarget[data->lastRectNum].rect = current[j];
						tch_updateTargetRcd(data, -1, ADD);
					}*/
					//TCH_PRINTF("add new!\r\n");
					data->g_lastTarget[data->lastRectNum].rect = current[j];
					tch_updateTargetRcd(data, -1, ADD);
				}
				
			}
			
		}
		//TCH_PRINTF("==============\r\n");
		tch_mergeRects(data);
		return flag;
	}
}

static void tch_updateTargetRcd(Tch_Data_t *data, int index, int type)
{
	if (type == REMOVE)
	{
		//第一个记录为0的问题在这里
		int i;
		if (data->lastRectNum == 1)
		{
			data->lastRectNum--;
			data->g_lastTarget[0].rect.x = 0;
			data->g_lastTarget[0].rect.y = 0;
			data->g_lastTarget[0].rect.width = 0;
			data->g_lastTarget[0].rect.height = 0;
			data->g_lastTarget[0].start = 0;
			data->g_lastTarget[0].end = 0;
			tch_updateTimer(&data->g_lastTarget[0].timer, CLEAR);
			return;
		}
		else
		{
			for (i = index; i < data->lastRectNum; i++)
			{
				if (i == data->lastRectNum - 1)
				{
					data->g_lastTarget[data->lastRectNum - 1].rect.x = 0;
					data->g_lastTarget[data->lastRectNum - 1].rect.y = 0;
					data->g_lastTarget[data->lastRectNum - 1].rect.width = 0;
					data->g_lastTarget[data->lastRectNum - 1].rect.height = 0;
					data->g_lastTarget[data->lastRectNum - 1].start = 0;
					data->g_lastTarget[data->lastRectNum - 1].end = 0;
					tch_updateTimer(&data->g_lastTarget[data->lastRectNum - 1].timer, CLEAR);
				}
				else
				{
					data->g_lastTarget[i] = data->g_lastTarget[i + 1];
				}
			}
			data->lastRectNum--;
		}
		return;
	}
	else
	{
		data->g_lastTarget[data->lastRectNum].start = 1;
		data->g_lastTarget[data->lastRectNum].end = 0;
		tch_updateTimer(&data->g_lastTarget[data->lastRectNum].timer, RESET);
		data->g_lastTarget[data->lastRectNum].o_start = data->g_lastTarget[data->lastRectNum].timer.start;
		data->g_lastTarget[data->lastRectNum].o_x = data->g_lastTarget[data->lastRectNum].rect.x;
		data->g_lastTarget[data->lastRectNum].o_width = data->g_lastTarget[data->lastRectNum].rect.width;
		data->lastRectNum++;
	}
}

static int tch_noneTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV)
{
	int isChange;
	if (data->lastRectNum==0)
	{
		//track_timerInit(data->nodeOutside);
		if (data->isAnalysing)
		{
			if (data->nodeOutside->start == (ULINT)-1)
			{
				track_timerStart(data->nodeOutside);//分析
			}
			if (data->analysis->standTimer.start != 0)
			{
				track_timerEnd(&data->analysis->standTimer);
			}
			if (data->analysis->moveTimer.start != 0)
			{
				track_timerEnd(&data->analysis->moveTimer);
			}
		}
		
		res->status = RETURN_TRACK_TCH_OUTSIDE;
		isChange = (res->status != data->tch_lastStatus);
		data->tch_lastStatus = RETURN_TRACK_TCH_OUTSIDE;
		data->slideTimer.deltaTime = 0;
		data->slideTimer.start = 0;
		data->pos_slide.center = -1;
		data->pos_slide.left = -1;
		data->pos_slide.right = -1;
		res->pos = -1;
		data->g_posIndex = -1;
		data->g_prevPosIndex = -1;
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		data->lastRectNum = 0;
		return isChange;
	}
	if (data->lastRectNum==1)
	{
		tch_updateFeatureRect(data);
		Track_Rect_t featureRect;
		featureRect.x = data->pos_slide.left * data->track_pos_width + data->g_tchWin.x;
		featureRect.y = data->g_anaWin.y;
		featureRect.width = data->track_pos_width * data->numOfSlide;
		featureRect.height = data->g_anaWin.height;
		tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);

		Track_Rect_t drawRect;
		drawRect.x = data->g_lastTarget[0].rect.x + data->g_anaWin.x;
		drawRect.y = data->g_lastTarget[0].rect.y + data->g_anaWin.y;
		drawRect.width = data->g_lastTarget[0].rect.width;
		drawRect.height = data->g_lastTarget[0].rect.height;
		tch_updateTimer(&data->g_lastTarget[0].timer,UPDATE);
		if ((data->g_lastTarget[0].timer.deltaTime) > params->threshold.stand)
		{
			if (data->isAnalysing)
			{
				if (data->analysis->standTimer.start==0)
				{
					track_timerStart(&data->analysis->standTimer);
				}
				if (data->analysis->moveTimer.start!=0)
				{
					track_timerEnd(&data->analysis->moveTimer);//分析
				}
			}
			
			
			res->status = RETURN_TRACK_TCH_MOVEINVIEW;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			if (data->g_lastTarget[0].end > 0)
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
			}
			else
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
			}
			//data->lastRectNum = s_rectCnt;
			return isChange;
		}
		else
		{
			res->status = RETURN_TRACK_TCH_MOVEOUTVIEW;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_MOVEOUTVIEW;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			if (data->g_lastTarget[0].end > 0)
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
			}
			else
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
			}
			//data->lastRectNum = s_rectCnt;
			return isChange;
		}
	}
	if (data->lastRectNum>1)
	{
		int i;
		Track_Rect_t drawRect;
		for (i = 0; i < data->lastRectNum; i++)
		{
			drawRect.x = data->g_lastTarget[i].rect.x + data->g_anaWin.x;
			drawRect.y = data->g_lastTarget[i].rect.y + data->g_anaWin.y;
			drawRect.width = data->g_lastTarget[i].rect.width;
			drawRect.height = data->g_lastTarget[i].rect.height;
			tch_updateTimer(&data->g_lastTarget[i].timer, UPDATE);
			if (data->g_lastTarget[0].end>0)
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
			}
			else
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
			}
		}
		if (data->isAnalysing)
		{
			if (data->nodeMultiple->start == 0)
			{
				track_timerStart(data->nodeMultiple);
			}
			if (data->analysis->standTimer.start != 0)
			{
				track_timerEnd(&data->analysis->standTimer);
			}
			if (data->analysis->moveTimer.start != 0)
			{
				track_timerEnd(&data->analysis->moveTimer);
			}
		}
		
		res->status = RETURN_TRACK_TCH_MULITY;
		isChange = (res->status != data->tch_lastStatus);
		data->tch_lastStatus = RETURN_TRACK_TCH_MULITY;
		res->pos = data->pos_slide.center + PRESET_ALIGN;
		/*data->slideTimer.deltaTime = 0;
		data->slideTimer.start = gettime();*/
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		return isChange;
	}
	//if (data->lastRectNum==0)
	//{
	//	res->status = RETURN_TRACK_TCH_OUTSIDE;
	//	isChange = (res->status != data->tch_lastStatus);
	//	data->tch_lastStatus = RETURN_TRACK_TCH_OUTSIDE;
	//	data->slideTimer.deltaTime = 0;
	//	data->slideTimer.start = 0;
	//	data->pos_slide.center = -1;
	//	data->pos_slide.left = -1;
	//	data->pos_slide.right = -1;
	//	res->pos = -1;
	//	data->g_posIndex = -1;
	//	data->g_prevPosIndex = -1;
	//	isChange = tch_return_maintain(&data->tch_timer, isChange);
	//	data->lastRectNum = 0;
	//	return isChange;
	//}
	//if (data->lastRectNum == 1)
	//{
	//	Track_Rect_t drawRect;
	//	drawRect.x = data->g_lastTarget[0].x + data->g_tchWin.x;
	//	drawRect.y = data->g_lastTarget[0].y + data->g_tchWin.y;
	//	drawRect.width = data->g_lastTarget[0].width;
	//	drawRect.height = data->g_lastTarget[0].height;
	//	if (data->tch_lastStatus == RETURN_TRACK_TCH_MOVEOUTVIEW)
	//	{
	//		data->slideTimer.finish = gettime();
	//		data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
	//		data->slideTimer.start = data->slideTimer.finish;
	//		if ((data->slideTimer.deltaTime) > params->threshold.stand)
	//		{
	//			res->pos = data->pos_slide.center + PRESET_ALIGN;
	//			res->status = RETURN_TRACK_TCH_MOVEINVIEW;
	//			isChange = (res->status != data->tch_lastStatus);
	//			data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
	//			isChange = tch_return_maintain(&data->tch_timer, isChange);
	//			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
	//			//data->lastRectNum = s_rectCnt;
	//			return isChange;
	//		}
	//		else
	//		{
	//			res->pos = data->pos_slide.center + PRESET_ALIGN;
	//			res->status = data->tch_lastStatus;
	//			isChange = (res->status != data->tch_lastStatus);
	//			isChange = tch_return_maintain(&data->tch_timer, isChange);
	//			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
	//			//data->lastRectNum = s_rectCnt;
	//			return isChange;
	//		}
	//	}
	//	else
	//	{
	//		res->pos = data->pos_slide.center + PRESET_ALIGN;
	//		res->status = data->tch_lastStatus;
	//		isChange = (res->status != data->tch_lastStatus);
	//		isChange = tch_return_maintain(&data->tch_timer, isChange);
	//		tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
	//		//data->lastRectNum = s_rectCnt;
	//		return isChange;
	//	}
	//}
	//if (data->lastRectNum > 1)
	//{
	//	int i;
	//	Track_Rect_t drawRect;
	//	for (i = 0; i < data->lastRectNum; i++)
	//	{
	//		drawRect.x = data->g_lastTarget[i].x + data->g_tchWin.x;
	//		drawRect.y = data->g_lastTarget[i].y + data->g_tchWin.y;
	//		drawRect.width = data->g_lastTarget[i].width;
	//		drawRect.height = data->g_lastTarget[i].height;
	//		tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
	//	}
	//	data->slideTimer.start = gettime();
	//	data->slideTimer.deltaTime = 0;
	//	res->pos = data->pos_slide.center + PRESET_ALIGN;
	//	res->status = data->tch_lastStatus;
	//	isChange = (res->status != data->tch_lastStatus);
	//	isChange = tch_return_maintain(&data->tch_timer, isChange);
	//	//data->lastRectNum = s_rectCnt;
	//	return isChange;
	//}
	return -1;
}

static int tch_multipleTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV)
{
	Track_Rect_t drawRect;
	
	tch_updateFeatureRect(data);
	int isChange;
	int i;
	for (i = 0; i < data->lastRectNum; i++)
	{
		drawRect.x = data->g_lastTarget[i].rect.x + data->g_anaWin.x;
		drawRect.y = data->g_lastTarget[i].rect.y + data->g_anaWin.y;
		drawRect.width = data->g_lastTarget[i].rect.width;
		drawRect.height = data->g_lastTarget[i].rect.height;
		tch_updateTimer(&data->g_lastTarget[i].timer, UPDATE);
		if (data->g_lastTarget[0].end > 0)
		{
			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
		}
		else
		{
			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
		}
	}
	if (data->isAnalysing)
	{
		if (data->nodeMultiple->start == 0)
		{
			track_timerStart(data->nodeMultiple);
		}
		if (data->analysis->standTimer.start != 0)
		{
			track_timerEnd(&data->analysis->standTimer);
		}
		if (data->analysis->moveTimer.start != 0)
		{
			track_timerEnd(&data->analysis->moveTimer);
		}
	}
	
	//track_timerUpdate(&data->analysis->standTimer);
	//track_timerUpdate(&data->analysis->moveTimer);

	res->status = RETURN_TRACK_TCH_MULITY;
	isChange = (res->status != data->tch_lastStatus);
	data->tch_lastStatus = RETURN_TRACK_TCH_MULITY;
	res->pos = data->pos_slide.center + PRESET_ALIGN;
	/*data->slideTimer.deltaTime = 0;
	data->slideTimer.start = gettime();*/
	isChange = tch_return_maintain(&data->tch_timer, isChange);
	return isChange;
}

static int tch_singleTarget(Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res,  itc_uchar *src, itc_uchar* pUV)
{
	//获取运动方向
	int direct = -1;
	int isChange;
	float dx = 0, dy = 0;
	Track_Rect_t rectTch = data->g_lastTarget[0].rect;
	track_calculateDirect_ROI(data->mhiMatTch, rectTch, &direct, &dx, &dy);
	Track_Rect_t drawRect;
	if (direct>-1)
	{
		//data->center = itcPoint(rectTch.x + (rectTch.width >> 1), rectTch.y + (rectTch.height >> 1));
		if (data->isAnalysing)
		{
			if (data->nodeMultiple->start!=0)
			{
				track_timerEnd(data->nodeMultiple);
				data->analysis->mlpTimer = track_timerIncrease(data->analysis->mlpTimer, &data->analysis->cntMultiple, data->nodeMultiple);
				//data->nodeMultiple = track_timerInit();
				memset(data->nodeMultiple, 0, sizeof(Analysis_Timer_t));
			}
		}
		
		tch_updatePosition(data);
		tch_updateFeatureRect(data);
		Track_Rect_t featureRect;
		featureRect.x = data->pos_slide.left * data->track_pos_width + data->g_tchWin.x;
		featureRect.y = data->g_anaWin.y;
		featureRect.width = data->track_pos_width * data->numOfSlide;
		featureRect.height = data->g_anaWin.height;
		tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);

		drawRect.x = data->g_lastTarget[0].rect.x + data->g_anaWin.x;
		drawRect.y = data->g_lastTarget[0].rect.y + data->g_anaWin.y;
		drawRect.width = data->g_lastTarget[0].rect.width;
		drawRect.height = data->g_lastTarget[0].rect.height;
		//TCH_PRINTF("End count:%d\r\n", data->g_lastTarget[0].end);

		if (data->isAnalysing)
		{
			memset(data->nodeOutside, -1, sizeof(Analysis_Timer_t));
			data->nodeOutside->deltatime = 0;
		}
		
		if ((data->g_lastTarget[0].timer.deltaTime) > params->threshold.stand)
		{
			if (data->isAnalysing)
			{
				if (data->analysis->standTimer.start==0)
				{
					track_timerStart(&data->analysis->standTimer);
				}
			
				if (data->analysis->moveTimer.start!=0)
				{
					track_timerEnd(&data->analysis->moveTimer);//分析
				}
			}
			
			
			res->status = RETURN_TRACK_TCH_MOVEINVIEW;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			if (data->g_lastTarget[0].end>0)
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
			}
			else
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
			}
			//data->lastRectNum = s_rectCnt;
			return isChange;
		}
		else
		{
			res->status = RETURN_TRACK_TCH_MOVEOUTVIEW;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_MOVEOUTVIEW;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			if (data->g_lastTarget[0].end > 0)
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
			}
			else
			{
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
			}
			//data->lastRectNum = s_rectCnt;
			return isChange;
		}
	}
	return -1;
}

static void tch_updatePosition(Tch_Data_t *data)
{
	int i;
	Tch_CamPosition_t *ptr;
	ptr = data->cam_pos;
	data->center = itcPoint(data->g_lastTarget[0].rect.x + (data->g_lastTarget[0].rect.width >> 1), data->g_lastTarget[0].rect.y + (data->g_lastTarget[0].rect.height >> 1));
	//获取当前运动框体所在的预置位
	if (data->center.x<=data->g_tchWin.x)
	{
		data->g_posIndex = 0;
		return;
	}
	int right = data->g_tchWin.x + data->g_tchWin.width;
	if (data->center.x >= right)
	{
		data->g_posIndex = data->numOfPos;
		return;
	}
	for (i = 0; i < data->numOfPos; i++)
	{
		if (ptr->left_pixel <= data->center.x&&data->center.x <= ptr->right_pixel)
		{
			if (data->g_prevPosIndex == -1)
			{
				data->g_prevPosIndex = ptr->index;
			}
			data->g_posIndex = ptr->index;
			break;
		}
		ptr++;
	}
	ptr = NULL;
}


#define TRACK_OUTSIDE_TIME 5000
int tch_track(itc_uchar *src, itc_uchar* pUV, TeaITRACK_Params *params, Tch_Data_t *data, Tch_Result_t *res)
{

	if (src==NULL || pUV==NULL|| params==NULL || data==NULL || res==NULL)
	{
		return -2;
	}
	res->pos = -1;
	res->status = RETURN_TRACK_TCH_NULL;
	int i = 0;


	if (data->g_count>0)
	{
		ITC_SWAP(data->currMatTch, data->prevMatTch, data->tempMatTch);
		ITC_SWAP(data->currMatBlk, data->prevMatBlk, data->tempMatBlk);
	}
	tchTrack_Copy_matData(data, src);

#ifdef _WIN32
	tchTrack_drawShow_imgData(data, src, pUV, &data->g_tchWin, &data->red_colour);
	tchTrack_drawShow_imgData(data, src, pUV, &data->g_blkWin, &data->red_colour);
#endif

	int s_contourRectTch = 0;//老师的轮廓数目
	int s_contourRectBlk = 0;//板书的轮廓数目
	Track_Rect_t s_rectsTch[100];
	Track_Rect_t s_rectsBlk[100];
	Track_Rect_t s_bigRects[100];//筛选出来的大面积运动物体
	//int s_maxdist = -1;//比较多个面积
	int s_rectCnt = 0;
	int isChange = -1;

	if (data->g_count>0)
	{
		data->g_count++;
		if (data->g_count>1000000)
		{
			data->g_count = 1;
		}
		//g_time = (double)cvGetTickCount();
		track_update_MHI(data->currMatTch, data->prevMatTch, data->mhiMatTch, 10, data->maskMatTch, 242);
		Track_Contour_t *contoursTch = NULL;
		itcClearMemStorage(data->storageTch);
		track_find_contours(data->maskMatTch, &contoursTch, data->storageTch);
		s_contourRectTch = track_filtrate_contours(&contoursTch, 20, s_rectsTch);

		track_update_MHI(data->currMatBlk, data->prevMatBlk, data->mhiMatBlk, 10, data->maskMatBlk, 235);
		Track_Contour_t *contoursBlk = NULL;
		itcClearMemStorage(data->storageBlk);
		track_find_contours(data->maskMatBlk, &contoursBlk, data->storageBlk);
		s_contourRectBlk = track_filtrate_contours(&contoursBlk, 10, s_rectsBlk);
		//Track_Rect_t drawRect;

		for (i = 0; i < s_contourRectTch; i++)
		{
			if (params->threshold.targetArea<s_rectsTch[i].width*s_rectsTch[i].height)
			{
				s_bigRects[s_rectCnt] = s_rectsTch[i];
				s_rectCnt++;
			}
		}

#ifdef WIN32
		int YUV420_type = TRACK_DRAW_YUV420P;
		Track_Size_t imgSize;
		imgSize.height = data->g_frameSize.height;
		imgSize.width = data->g_frameSize.width;
		Track_Point_t point1, point2;
		point1.x = 0, point1.y = params->threshold.outside + data->g_anaWin.y;
		point2.x = imgSize.width, point2.y = params->threshold.outside + data->g_anaWin.y;
		track_draw_line(src, pUV, &imgSize, &point1, &point2, &data->black_colour, YUV420_type);
#endif
		
		
		//判断是否出发板书并且更新教师的位置
		int blk_flag = tch_isBlackBoard(s_contourRectBlk, s_rectCnt, s_rectsBlk, s_bigRects, data, params, src, pUV);
		if (blk_flag == TRACK_TRUE)
		{
			res->status = RETURN_TRACK_TCH_BLACKBOARD;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_BLACKBOARD;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);

			Track_Rect_t drawRect;

			for (i = 0; i < data->lastRectNum; i++)
			{
				drawRect.x = data->g_lastTarget[i].rect.x + data->g_anaWin.x;
				drawRect.y = data->g_lastTarget[i].rect.y + data->g_anaWin.y;
				drawRect.width = data->g_lastTarget[i].rect.width;
				drawRect.height = data->g_lastTarget[i].rect.height;
				tch_updateTimer(&data->g_lastTarget[i].timer, UPDATE);
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
			}
			tch_matchRects(s_bigRects, s_rectCnt, data, params);
			return isChange;
		}
		else
		{
			if (s_rectCnt>0)//检测到运动目标的情况
			{
				int tch_flag = tch_matchRects(s_bigRects, s_rectCnt, data,params);
				if (tch_flag == TRACK_FALSE)
				{
					TCH_PRINTF("Data Error, can't match!\r\n");
					return -1;
				}
				tch_analyzeOutside(data, params);
				if (data->lastRectNum == 1)
				{
					return tch_singleTarget(data, params, res, src, pUV);
				}
				if (data->lastRectNum > 1)
				{
					return tch_multipleTarget(data, params, res, src, pUV);
				}
				if (data->lastRectNum == 0)
				{
					return tch_noneTarget(data, params, res, src, pUV);
				}
			}
			else
			{
				return tch_noneTarget(data, params, res, src, pUV);
			}
		}
	}
	else
	{
		if (!data->currMatTch->data.ptr||!data->currMatBlk->data.ptr)
		{
			return -2;
		}
		if (data->g_count==0)
		{
			data->tch_timer.start = gettime();
		}
		data->g_count++;
		isChange = (res->status != data->tch_lastStatus);
		data->tch_lastStatus = res->status;
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		return isChange;
	}
	isChange = (res->status != data->tch_lastStatus);
	data->tch_lastStatus = res->status;
	isChange = tch_return_maintain(&data->tch_timer, isChange);
	return isChange;
}

int tch_calculateDirect_TCH(Itc_Mat_t* src, Track_Rect_t roi)
{
	int i = 0;
	int j = 0;
	int direct = 0;;
	int sum_gradientV = 0;		//垂直方向梯度
	int sum_gradientH = 0;		//水平方向

	int x1 = roi.x;
	int y1 = roi.y;
	int x2 = roi.x + roi.width;
	int y2 = roi.y + roi.height;

	int step = src->step;
	char *img0 = (char*)(src->data.ptr + step*y1);
	char *img = (char*)(src->data.ptr + step*(y1 + 1));

	for (i = y1; i < y2 - 1; i++)
	{
		for (j = x1; j < x2 - 1; j++)
		{
			if (img0[j] != 0)
			{
				if (img[j] != 0)
					sum_gradientV += img0[j] - img[j];
				if (img0[j + 1] != 0)
					sum_gradientH += img0[j] - img0[j + 1];
			}
		}
		img0 = img;
		img += step;
	}

	int threshold = roi.width*roi.height / 10;
	if (abs(sum_gradientV) > abs(sum_gradientH))
	{
		if (sum_gradientV > threshold)
		{
			direct = 2;
		}
		else if (sum_gradientV < -threshold)
		{
			direct = 1;
		}
	}
	else
	{
		if (sum_gradientH > threshold)
		{
			direct = 3;
		}
		else if (sum_gradientH < -threshold)
		{
			direct = 4;
		}
	}
	return direct;
	//printf("位置：%d,%d,大小：%d,%d 垂直梯度：%d,水平梯度：%d\n", x1, y1, roi.width, roi.height,sum_gradientV, sum_gradientH);
}

void tch_trackDestroy(Tch_Data_t *data)
{
	if (data->g_count > 0)
	{
		itcReleaseMemStorage(&data->storageTch);
		itcReleaseMemStorage(&data->storageBlk);
		itcReleaseMemStorage(&data->storage);

		itc_release_mat(&data->currMatTch);
		itc_release_mat(&data->prevMatTch);
		itc_release_mat(&data->mhiMatTch);
		itc_release_mat(&data->maskMatTch);

		itc_release_mat(&data->currMatBlk);
		itc_release_mat(&data->prevMatBlk);
		itc_release_mat(&data->mhiMatBlk);
		itc_release_mat(&data->maskMatBlk);

		free(data->cam_pos);
		data->cam_pos = NULL;
		//DestroyQueue(data->tch_queue);
	}
	else
	{
		return;
	}
}

int tch_Init(TeaITRACK_Params *params, Tch_Data_t *data)
{
	if (params==NULL||data==NULL)
	{
		data->sysData.callbackmsg_func("params err.");
		return -1;
	}

	tch_trackDestroy(data);

	if (params->isSetParams==0)
	{
		params->frame.width = TRACK_DEFAULT_WIDTH;
		params->frame.height = TRACK_DEFAULT_HEIGHT;

		params->tch.x = TRACK_DEFAULT_TCH_X;
		params->tch.y = TRACK_DEFAULT_TCH_Y;
		params->tch.width = TRACK_DEFAULT_TCH_W;
		params->tch.height = TRACK_DEFAULT_TCH_H;

		params->blk.x = TRACK_DEFAULT_BLK_X;
		params->blk.y = TRACK_DEFAULT_BLK_Y;
		params->blk.width = TRACK_DEFAULT_BLK_W;
		params->blk.height = TRACK_DEFAULT_BLK_H;

		params->threshold.stand = TRACK_STAND_THRESHOLD;
		params->threshold.outside = TRACK_TCHOUTSIDE_THRESHOLD;
		params->threshold.targetArea = TRACK_TARGETAREA_THRESHOLD;

		params->numOfPos = TRACK_NUMOF_POSITION;
		params->numOfSlide = TRACK_SLIDE_WIDTH;

		//return 0;
	}
	//初始化帧的大小
	if (params->frame.width != 480 || params->frame.height != 264)
	{
		data->sysData.callbackmsg_func("frame size err.");
		return -1;
	}
	data->g_frameSize.width = params->frame.width;
	data->g_frameSize.height = params->frame.height;
	//初始化教师框
	if (params->tch.width<0 || params->tch.height<0)
	{
		data->sysData.callbackmsg_func("teacher window size err.");
		return -1;
	}
	else if (params->tch.width>params->frame.width || params->tch.height>params->frame.height)
	{
		data->sysData.callbackmsg_func("teacher window size err.");
		return -1;
	}
	else if (params->tch.x<0 || params->tch.x>params->frame.width || params->tch.y<0 || params->tch.y>params->frame.height)
	{
		data->sysData.callbackmsg_func("teacher window size err.");
		return -1;
	}
	else
	{
		data->g_tchWin.x = params->tch.x;
		data->g_tchWin.y = params->tch.y;
		data->g_tchWin.width = params->tch.width;
		data->g_tchWin.height = params->tch.height;
	}
	//初始化板书框体
	if (params->blk.width < 0 || params->blk.height < 0)
	{
		data->sysData.callbackmsg_func("blackboard window size err.");
		return -1;
	}
	else if (params->blk.width > params->frame.width || params->blk.height > params->frame.height)
	{
		data->sysData.callbackmsg_func("blackboard window size err.");
		return -1;
	}
	else if (params->blk.x < 0 || params->blk.x > params->frame.width || params->blk.y < 0 || params->blk.y > params->frame.height)
	{
		data->sysData.callbackmsg_func("blackboard window size err.");
		return -1;
	}
	else
	{
		data->g_blkWin.x = params->blk.x;
		data->g_blkWin.y = params->blk.y;
		data->g_blkWin.width = params->blk.width;
		data->g_blkWin.height = params->blk.height;
	}
	//初始化阈值
	if (params->threshold.stand <= 0 || params->threshold.targetArea <= 0 || params->threshold.outside <= 0)
	{
		data->sysData.callbackmsg_func("threshold err.");
		return -1;
	}
	if (params->numOfPos<params->numOfSlide||params->numOfPos<=0||params->numOfSlide<=0)
	{
		data->sysData.callbackmsg_func("camera position err.");
		return -1;
	}
	else
	{
		data->numOfPos = params->numOfPos;
		data->numOfSlide = params->numOfSlide;
	}
	/*else
	{
	track_standThreshold = params->threshold.stand;
	track_targetAreaThreshold = params->threshold.targetArea;
	track_tchOutsideThreshold = params->threshold.outside;
	}*/
	if (tch_trackInit(data)<0)
	{
		return -1;
	}

	//初始化绘制颜色
	data->pink_colour = colour_RGB2YUV(255, 0, 255);	/*粉红*/
	data->blue_colour = colour_RGB2YUV(0, 0, 255);		/*纯蓝*/
	data->lilac_colour = colour_RGB2YUV(155, 155, 255);/*淡紫*/
	data->green_colour = colour_RGB2YUV(0, 255, 0);	/*纯绿*/
	data->red_colour = colour_RGB2YUV(255, 0, 0);		/*纯红*/
	data->dullred_colour = colour_RGB2YUV(127, 0, 0);	/*暗红*/
	data->yellow_colour = colour_RGB2YUV(255, 255, 0);	/*纯黄*/
	data->black_colour = colour_RGB2YUV(0, 0, 0);
	return 0;
}
