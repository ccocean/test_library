#include "tch_track.h"

#define PRESET_ALIGN 10

static void tchTrack_Copy_matData(Tch_Data_t* datas, itc_uchar* srcData);
static void tchTrack_drawShow_imgData(Tch_Data_t* interior_params, itc_uchar* imageData, itc_uchar* bufferuv, Track_Rect_t *rect, Track_Colour_t *color);
static int tch_return_maintain(Tch_Timer_t *tch_timer, int in);
static int tch_isBlackBoard(int numBlk, int numTch, Track_Rect_t *rectBlk, Track_Rect_t rectTch, Tch_Data_t *data, TeaITRACK_Params *params, itc_uchar *src, itc_uchar* pUV);
static int tch_singleTarget(Track_Rect_t rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static int tch_multipleTarget(Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static int tch_noneTarget(Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV);
static void tch_updatePosition(Tch_Data_t *data);
static void tchTrack_Copy_matData(Tch_Data_t* datas, itc_uchar* srcData)
{
	//ITC_FUNCNAME("FUNCNAME:stuTrack_resizeCopy_matData\n");
	int y = 0;
	int tch_step = datas->g_tchWin.width;
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
	itc_uchar* src_tch = srcData + src_step*datas->g_tchWin.y + datas->g_tchWin.x;
	for (y = 0; y < datas->g_tchWin.height; y++)
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
		data->sysData.callbackmsg_func("Tch_Data_t err.");
		return -1;
	}
	data->track_pos_width = data->g_frameSize.width / data->numOfPos;

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

	data->cam_pos = calloc(data->numOfPos, sizeof(Tch_CamPosition_t));
	Tch_CamPosition_t *ptr = data->cam_pos;

	int cnt = 0;
	int i = 0;
	while (cnt<data->numOfPos)
	{
		ptr->index = i / data->track_pos_width;
		ptr->left_pixel = i;
		ptr->right_pixel = i + data->track_pos_width;
		i += data->track_pos_width;
		ptr++;
		cnt++;
	}
	
	ptr = NULL;

	//data->srcMat = itc_create_mat(data->g_frameSize.height, data->g_frameSize.width, ITC_8UC1);

	data->prevMatTch = itc_create_mat(data->g_tchWin.height, data->g_tchWin.width, ITC_8UC1);
	data->prevMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->tempMatTch = NULL;
	data->tempMatBlk = NULL;

	data->currMatTch = itc_create_mat(data->g_tchWin.height, data->g_tchWin.width, ITC_8UC1);
	data->mhiMatTch = itc_create_mat(data->g_tchWin.height, data->g_tchWin.width, ITC_8UC1);
	data->maskMatTch = itc_create_mat(data->g_tchWin.height, data->g_tchWin.width, ITC_8UC1);
	data->currMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->mhiMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);
	data->maskMatBlk = itc_create_mat(data->g_blkWin.height, data->g_blkWin.width, ITC_8UC1);

	data->storage = itcCreateMemStorage(0);
	data->storageTch = itcCreateChildMemStorage(data->storage);
	data->storageBlk = itcCreateChildMemStorage(data->storage);

	//data->tch_queue = InitQueue();
	//data->callbackmsg_func = printf;

	return 0;
}

static void tchTrack_drawShow_imgData(Tch_Data_t* interior_params, itc_uchar* imageData, itc_uchar* bufferuv, Track_Rect_t *rect, Track_Colour_t *color)
{
	Track_Size_t srcimg_size = { interior_params->sysData.width, interior_params ->sysData.height};	//ԭʼͼ���С
#ifdef _WIN32
	int YUV420_type = TRACK_DRAW_YUV420P;
#endif
#ifndef _WIN32
	int YUV420_type = TRACK_DRAW_YUV420SP;
#endif
	track_draw_rectangle(imageData, bufferuv, &srcimg_size, rect, color, YUV420_type);
}


#define TRACK_MAINTAIN_TIME 2000//ά������ֵ��ʱ�䣬2���ӷ���һ�����
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
	//����λ��
	data->g_prevPosIndex = data->g_posIndex;
	//pos_1 = data->center;
	if (data->g_posIndex == data->pos_slide.right || data->g_posIndex == data->pos_slide.left)//�������ߵ�����ı�Ե���͸��»����൱�ڵ�ȫ���ƶ���̨������޸�Ϊ�ߵ���Եǰһ��Ԥ��λ�͸��¾�ͷ
	{
		if (data->g_posIndex < data->pos_slide.width)//���
		{
			if (data->g_posIndex == 0)//�������ʱά��ʱ��
			{
				data->slideTimer.finish = gettime();
				data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
				data->slideTimer.start = data->slideTimer.finish;
			}
			else//�������ʱ��
			{
				data->slideTimer.finish = 0;
				data->slideTimer.deltaTime = 0;
				data->slideTimer.start = gettime();
			}
			data->pos_slide.center = data->pos_slide.width;
			data->pos_slide.left = data->pos_slide.center - data->pos_slide.width;
			data->pos_slide.right = data->pos_slide.center + data->pos_slide.width;
		}
		else if (data->g_posIndex > data->numOfPos - 1 - data->pos_slide.width)//�Ҷ�
		{
			if (data->g_posIndex == data->numOfPos - 1)//�����Ҷ�ʱά��ʱ��
			{
				data->slideTimer.finish = gettime();
				data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
				data->slideTimer.start = data->slideTimer.finish;
			}
			else//�������ʱ��
			{
				data->slideTimer.finish = 0;
				data->slideTimer.deltaTime = 0;
				data->slideTimer.start = gettime();
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

			data->slideTimer.finish = 0;
			data->slideTimer.deltaTime = 0;
			data->slideTimer.start = gettime();
		}
	}
	else if (data->pos_slide.left > data->g_posIndex || data->g_posIndex > data->pos_slide.right)
	{
		//��׷�ٵ���Ŀ���Ѿ�����һ֡��������������̸��»���
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
		data->slideTimer.finish = 0;
		data->slideTimer.deltaTime = 0;
		data->slideTimer.start = gettime();
	}
	else//����д��ͷ��Χ����ά��ʱ��
	{
		data->slideTimer.finish = gettime();
		data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
		data->slideTimer.start = data->slideTimer.finish;
	}
}

static int tch_isBlackBoard(int numBlk, int numTch, Track_Rect_t *rectBlk, Track_Rect_t rectTch, Tch_Data_t *data, TeaITRACK_Params *params,  itc_uchar *src, itc_uchar* pUV)
{
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
		if (1 == numTch)
		{
			//��ȡ�˶�����
			int direct = -1;
			direct = tch_calculateDirect_TCH(data->mhiMatTch, rectTch);
			if (direct > -1)
			{
				data->center = itcPoint(rectTch.x + (rectTch.width >> 1), rectTch.y + (rectTch.height >> 1));
				drawRect.x = rectTch.x + data->g_tchWin.x;
				drawRect.y = rectTch.y + data->g_tchWin.y;
				drawRect.width = rectTch.width;
				drawRect.height = rectTch.height;
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);

				Tch_CamPosition_t *ptr;
				ptr = data->cam_pos;
				//��ȡ��ǰ�˶��������ڵ�Ԥ��λ
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
			if ((abs(data->g_prevPosIndex - data->g_posIndex) > data->pos_slide.width + 1) || (data->center.y > params->threshold.outside))
			{
				data->g_posIndex = data->g_prevPosIndex;
			}
			else
			{
				tch_updateFeatureRect(data);
				//res->pos = data->pos_slide.center + PRESET_ALIGN;
			}
		}
		return TRACK_TRUE;
	}
	else
		return TRACK_FALSE;
}

static int tch_matchRects(Track_Rect_t current, Tch_Data_t *data)
{
	if (data->lastRectNum==0)
	{
		return TRACK_FALSE;
	}
	int i = 0;
	int result = TRACK_FALSE;
	for (i = 0; i < data->lastRectNum; i++)
	{
		result = track_intersect_rect(&current, &data->g_lastTarget[i], -10);
		if (result == TRACK_TRUE)
			break;
	}
	if (result == TRACK_TRUE)
	{
		//data->sysData.callbackmsg_func("%d\n", i);
		return i;
	}
	else
		return TRACK_FALSE;
}

#define ADD 1
#define REMOVE 0
static void tch_updateTargetRcd(Tch_Data_t *data, int index, int type, Track_Rect_t addone)
{
	if (type == ADD)
	{
		data->g_lastTarget[data->lastRectNum] = addone;
		data->lastRectNum++;
		return;
	}
	if (type == REMOVE)
	{
		//��һ����¼Ϊ0������������
		int i;
		if (data->lastRectNum == 1)
		{
			data->lastRectNum--;
			data->g_lastTarget[0].x = 0;
			data->g_lastTarget[0].y = 0;
			data->g_lastTarget[0].width = 0;
			data->g_lastTarget[0].height = 0;
		}
		else
		{
			for (i = index; i < data->lastRectNum; i++)
			{
				if (i == data->lastRectNum - 1)
				{
					data->g_lastTarget[i].x = 0;
					data->g_lastTarget[i].y = 0;
					data->g_lastTarget[i].width = 0;
					data->g_lastTarget[i].height = 0;
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
}

static void checkCount(Tch_Data_t *data)
{
	int i;
	int cnt = 0;
	for (i = 0; i < data->lastRectNum; i++)
	{
		if (data->g_lastTarget[i].width>0)
		{
			cnt++;
		}
	}
	if (cnt!=data->lastRectNum)
	{
		TCH_PRINTF("count error!");
	}
}

static int tch_noneTarget(Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV)
{
	int isChange;
	if (data->lastRectNum==0)
	{
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
	if (data->lastRectNum == 1)
	{
		Track_Rect_t drawRect;
		drawRect.x = data->g_lastTarget[0].x + data->g_tchWin.x;
		drawRect.y = data->g_lastTarget[0].y + data->g_tchWin.y;
		drawRect.width = data->g_lastTarget[0].width;
		drawRect.height = data->g_lastTarget[0].height;
		if (data->tch_lastStatus == RETURN_TRACK_TCH_MOVEOUTVIEW)
		{
			data->slideTimer.finish = gettime();
			data->slideTimer.deltaTime += data->slideTimer.finish - data->slideTimer.start;
			data->slideTimer.start = data->slideTimer.finish;
			if ((data->slideTimer.deltaTime) > params->threshold.stand)
			{
				res->pos = data->pos_slide.center + PRESET_ALIGN;
				res->status = RETURN_TRACK_TCH_MOVEINVIEW;
				isChange = (res->status != data->tch_lastStatus);
				data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
				isChange = tch_return_maintain(&data->tch_timer, isChange);
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
				//data->lastRectNum = s_rectCnt;
				return isChange;
			}
			else
			{
				res->pos = data->pos_slide.center + PRESET_ALIGN;
				res->status = data->tch_lastStatus;
				isChange = (res->status != data->tch_lastStatus);
				isChange = tch_return_maintain(&data->tch_timer, isChange);
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
				//data->lastRectNum = s_rectCnt;
				return isChange;
			}
		}
		else
		{
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			res->status = data->tch_lastStatus;
			isChange = (res->status != data->tch_lastStatus);
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
			//data->lastRectNum = s_rectCnt;
			return isChange;
		}
	}
	if (data->lastRectNum > 1)
	{
		int i;
		Track_Rect_t drawRect;
		for (i = 0; i < data->lastRectNum; i++)
		{
			drawRect.x = data->g_lastTarget[i].x + data->g_tchWin.x;
			drawRect.y = data->g_lastTarget[i].y + data->g_tchWin.y;
			drawRect.width = data->g_lastTarget[i].width;
			drawRect.height = data->g_lastTarget[i].height;
			tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
		}
		data->slideTimer.start = gettime();
		data->slideTimer.deltaTime = 0;
		res->pos = data->pos_slide.center + PRESET_ALIGN;
		res->status = data->tch_lastStatus;
		isChange = (res->status != data->tch_lastStatus);
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		//data->lastRectNum = s_rectCnt;
		return isChange;
	}
}

static int tch_multipleTarget(Track_Rect_t *rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res, itc_uchar *src, itc_uchar* pUV)
{
	int i, index = -1;
	Track_Rect_t drawRect;
	Track_Point_t center;
	int isChange;
	for (i = 0; i < data->lastRectNum; i++)
	{
		center = itcPoint(rectTch[i].x + (rectTch[i].width >> 1), rectTch[i].y + (rectTch[i].height >> 1));
		index = tch_matchRects(rectTch[i], data);
		if (index>=0)
		{
			if (center.y>params->threshold.outside)
			{
				drawRect.x = rectTch[i].x + data->g_tchWin.x;
				drawRect.y = rectTch[i].y + data->g_tchWin.y;
				drawRect.width = rectTch[i].width;
				drawRect.height = rectTch[i].height;
				tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
				tch_updateTargetRcd(data, i, REMOVE, drawRect);
			}
			else
			{
				data->g_lastTarget[i] = rectTch[i];
			}
			
		}
		else
		{
			data->g_lastTarget[data->lastRectNum] = rectTch[i];
			data->lastRectNum++;
		}
		drawRect.x = rectTch[i].x + data->g_tchWin.x;
		drawRect.y = rectTch[i].y + data->g_tchWin.y;
		drawRect.width = rectTch[i].width;
		drawRect.height = rectTch[i].height;
		tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
	}
	if (data->lastRectNum>1)
	{
		res->status = RETURN_TRACK_TCH_MULITY;
		isChange = (res->status != data->tch_lastStatus);
		data->tch_lastStatus = RETURN_TRACK_TCH_MULITY;
		res->pos = data->pos_slide.center + PRESET_ALIGN;
		/*data->slideTimer.deltaTime = 0;
		data->slideTimer.start = gettime();*/
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		return isChange;
	}
	else if (data->lastRectNum==1)
	{
		data->center = itcPoint(rectTch[0].x + (rectTch[0].width >> 1), rectTch[0].y + (rectTch[0].height >> 1));
		tch_updatePosition(data);
		Track_Rect_t featureRect;
		featureRect.x = data->pos_slide.left * 48;
		featureRect.y = data->g_tchWin.y;
		featureRect.width = 48 * data->numOfSlide;
		featureRect.height = data->g_tchWin.height;

		tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);
		tch_updateFeatureRect(data);
		res->status = RETURN_TRACK_TCH_MOVEOUTVIEW;
		isChange = (res->status != data->tch_lastStatus);
		data->tch_lastStatus = RETURN_TRACK_TCH_MOVEOUTVIEW;
		res->pos = data->pos_slide.center + PRESET_ALIGN;
		isChange = tch_return_maintain(&data->tch_timer, isChange);
		tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
		//data->lastRectNum = s_rectCnt;
		return isChange;
	}
	else
	{
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
}

static int tch_singleTarget(Track_Rect_t rectTch, Tch_Data_t *data, TeaITRACK_Params *params, Tch_Result_t *res,  itc_uchar *src, itc_uchar* pUV)
{
	//��ȡ�˶�����
	int direct = -1;
	int isChange;
	float dx = 0, dy = 0;
	track_calculateDirect_ROI(data->mhiMatTch, rectTch, &direct, &dx, &dy);
	Track_Rect_t drawRect;
	
	if (direct>-1)
	{
		data->center = itcPoint(rectTch.x + (rectTch.width >> 1), rectTch.y + (rectTch.height >> 1));
		
		tch_updatePosition(data);

		//��⵱ǰĿ���֮ǰĿ���ƥ�����
		int index = -1;
		index = tch_matchRects(rectTch, data);
		drawRect.x = rectTch.x + data->g_tchWin.x;
		drawRect.y = rectTch.y + data->g_tchWin.y;
		drawRect.width = rectTch.width;
		drawRect.height = rectTch.height;
		//data->g_lastTarget[index] = rectTch;
		//tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
		if (index>=0)//��ƥ���Ŀ��
		{
			data->g_lastTarget[index] = rectTch;
			if (data->lastRectNum==1)
			{
				if (data->center.y>params->threshold.outside)
				{
					TCH_PRINTF("remove one\r\n");
					tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
					tch_updateTargetRcd(data, index, REMOVE, drawRect);
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
				else
				{
					TCH_PRINTF("match one\r\n");
					tch_updateFeatureRect(data);
					Track_Rect_t featureRect;
					featureRect.x = data->pos_slide.left * 48;
					featureRect.y = data->g_tchWin.y;
					featureRect.width = 48 * data->numOfSlide;
					featureRect.height = data->g_tchWin.height;

					tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);

					if ((data->slideTimer.deltaTime) > params->threshold.stand)
					{
						res->status = RETURN_TRACK_TCH_MOVEINVIEW;
						isChange = (res->status != data->tch_lastStatus);
						data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
						res->pos = data->pos_slide.center + PRESET_ALIGN;
						isChange = tch_return_maintain(&data->tch_timer, isChange);
						tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
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
						tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
						//data->lastRectNum = s_rectCnt;
						return isChange;
					}
				}
			}
			if (data->lastRectNum>1)
			{
				int i;
				data->g_lastTarget[index] = rectTch;
				if (data->center.y > params->threshold.outside)
				{
					TCH_PRINTF("remove one multiple\r\n");
					tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->black_colour);
					tch_updateTargetRcd(data, index, REMOVE, drawRect);
					/*res->status = RETURN_TRACK_TCH_OUTSIDE;
					isChange = (res->status != data->tch_lastStatus);
					data->tch_lastStatus = RETURN_TRACK_TCH_OUTSIDE;*/

					/*data->slideTimer.deltaTime = 0;
					data->slideTimer.start = 0;
					data->pos_slide.center = -1;
					data->pos_slide.left = -1;
					data->pos_slide.right = -1;
					res->pos = -1;
					data->g_posIndex = -1;
					data->g_prevPosIndex = -1;
					isChange = tch_return_maintain(&data->tch_timer, isChange);*/
					//data->lastRectNum = 0;
					//return isChange;
				}
				if (data->lastRectNum>1)
				{
					TCH_PRINTF("match multiple\r\n");
					for (i = 0; i < data->lastRectNum; i++)
					{
						drawRect.x = data->g_lastTarget[i].x + data->g_tchWin.x;
						drawRect.y = data->g_lastTarget[i].y + data->g_tchWin.y;
						drawRect.width = data->g_lastTarget[i].width;
						drawRect.height = data->g_lastTarget[i].height;
						tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
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
				if (data->lastRectNum==1)
				{
					data->center = itcPoint(data->g_lastTarget[0].x + (data->g_lastTarget[0].width >> 1), data->g_lastTarget[0].y + (data->g_lastTarget[0].height >> 1));
					tch_updatePosition(data);
					tch_updateFeatureRect(data);
					Track_Rect_t featureRect;
					featureRect.x = data->pos_slide.left * 48;
					featureRect.y = data->g_tchWin.y;
					featureRect.width = 48 * data->numOfSlide;
					featureRect.height = data->g_tchWin.height;

					tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);

					drawRect.x = data->g_lastTarget[0].x + data->g_tchWin.x;
					drawRect.y = data->g_lastTarget[0].y + data->g_tchWin.y;
					drawRect.width = data->g_lastTarget[0].width;
					drawRect.height = data->g_lastTarget[0].height;

					if ((data->slideTimer.deltaTime) > params->threshold.stand)
					{
						res->status = RETURN_TRACK_TCH_MOVEINVIEW;
						isChange = (res->status != data->tch_lastStatus);
						data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
						res->pos = data->pos_slide.center + PRESET_ALIGN;
						isChange = tch_return_maintain(&data->tch_timer, isChange);
						tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
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
						tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
						//data->lastRectNum = s_rectCnt;
						return isChange;
					}
				}
			}
		}
		else//û��ƥ��
		{
			if (data->lastRectNum==0)//��ǰĿ�����Ϊ�㣬��Ϊ����Ŀ��
			{
				TCH_PRINTF("add one\r\n");
				data->g_lastTarget[data->lastRectNum] = rectTch;
				data->lastRectNum++;

				tch_updateFeatureRect(data);
				Track_Rect_t featureRect;
				featureRect.x = data->pos_slide.left * 48;
				featureRect.y = data->g_tchWin.y;
				featureRect.width = 48 * data->numOfSlide;
				featureRect.height = data->g_tchWin.height;

				tchTrack_drawShow_imgData(data, src, pUV, &featureRect, &data->blue_colour);

				if ((data->slideTimer.deltaTime) > params->threshold.stand)
				{
					res->status = RETURN_TRACK_TCH_MOVEINVIEW;
					isChange = (res->status != data->tch_lastStatus);
					data->tch_lastStatus = RETURN_TRACK_TCH_MOVEINVIEW;
					res->pos = data->pos_slide.center + PRESET_ALIGN;
					isChange = tch_return_maintain(&data->tch_timer, isChange);
					tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->pink_colour);
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
					tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->yellow_colour);
					//data->lastRectNum = s_rectCnt;
					return isChange;
				}
			}
			if (data->lastRectNum >= 1)//��̨�����˵�û��ƥ�䣬��Ϊ�Ƕ�Ŀ��
			{
				TCH_PRINTF("add multiple\r\n");
				int i;
				data->g_lastTarget[data->lastRectNum] = rectTch;
				data->lastRectNum++;
				for (i = 0; i < data->lastRectNum; i++)
				{
					drawRect.x = data->g_lastTarget[i].x + data->g_tchWin.x;
					drawRect.y = data->g_lastTarget[i].y + data->g_tchWin.y;
					drawRect.width = data->g_lastTarget[i].width;
					drawRect.height = data->g_lastTarget[i].height;
					tchTrack_drawShow_imgData(data, src, pUV, &drawRect, &data->lilac_colour);
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
		}	
	}
}

static void tch_updatePosition(Tch_Data_t *data)
{
	int i;
	Tch_CamPosition_t *ptr;
	ptr = data->cam_pos;
	//��ȡ��ǰ�˶��������ڵ�Ԥ��λ
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

//static void tch_mergeRects()
//{
//	int i = 0, j = 0;
//	for (i = 0; i < count_rect; i++)
//	{
//		for (j = 0; j < count_rect; j++)
//		{
//			if (i != j)
//			{
//				if (track_intersect_rect(stuTrack_rect_arr + i, stuTrack_rect_arr + j, EXPAND_STUTRACK_INTERSECT_RECT))	//�ж��Ƿ��ཻ������ཻ��ֱ�Ӻϲ�
//				{
//					count_rect--;
//					*(stuTrack_rect_arr + j) = *(stuTrack_rect_arr + count_rect);
//					j--;
//				}
//			}
//		}
//	}
//}

#define TRACK_OUTSIDE_TIME 5000
int tch_track(itc_uchar *src, itc_uchar* pUV, TeaITRACK_Params *params, Tch_Data_t *data, Tch_Result_t *res)
{

	if (src==NULL || pUV==NULL|| params==NULL || data==NULL || res==NULL)
	{
		return -2;
	}
	res->pos = -1;
	res->status = RETURN_TRACK_TCH_NULL;
	int i = 0, j = 0;


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

	int s_contourRectTch = 0;//��ʦ��������Ŀ
	int s_contourRectBlk = 0;//�����������Ŀ
	Track_Rect_t s_rectsTch[100];
	Track_Rect_t s_rectsBlk[100];
	Track_Rect_t s_bigRects[100];//ɸѡ�����Ĵ�����˶�����
	int s_maxdist = -1;//�Ƚ϶�����
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
		s_contourRectBlk = track_filtrate_contours(&contoursBlk, 20, s_rectsBlk);
		Track_Rect_t drawRect;

		for (i = 0; i < s_contourRectTch; i++)
		{
			if (params->threshold.targetArea<s_rectsTch[i].width*s_rectsTch[i].height)
			{
				s_bigRects[s_rectCnt] = s_rectsTch[i];
				s_rectCnt++;
			}
		}

		//�ж��Ƿ�������鲢�Ҹ��½�ʦ��λ��
		int blk_flag = tch_isBlackBoard(s_contourRectBlk, s_rectCnt, &s_rectsBlk, s_bigRects[0],data,params,src,pUV);
		if (blk_flag == TRACK_TRUE)
		{
			res->status = RETURN_TRACK_TCH_BLACKBOARD;
			isChange = (res->status != data->tch_lastStatus);
			data->tch_lastStatus = RETURN_TRACK_TCH_BLACKBOARD;
			res->pos = data->pos_slide.center + PRESET_ALIGN;
			isChange = tch_return_maintain(&data->tch_timer, isChange);
			return isChange;
		}
		else
		{
			if (0 == s_rectCnt)
			{
				return tch_noneTarget(&s_bigRects, data, params, res, src, pUV);
			}
			else if (1 < s_rectCnt)
			{
				//return tch_multipleTarget(&s_bigRects, data, params, res, src, pUV);
				return 0;
			}
			else
			{
				return tch_singleTarget(s_bigRects[0], data, params, res, src, pUV);
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
	int sum_gradientV = 0;		//��ֱ�����ݶ�
	int sum_gradientH = 0;		//ˮƽ����

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
	//printf("λ�ã�%d,%d,��С��%d,%d ��ֱ�ݶȣ�%d,ˮƽ�ݶȣ�%d\n", x1, y1, roi.width, roi.height,sum_gradientV, sum_gradientH);
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
	//��ʼ��֡�Ĵ�С
	if (params->frame.width != 480 || params->frame.height != 264)
	{
		data->sysData.callbackmsg_func("frame size err.");
		return -1;
	}
	data->g_frameSize.width = params->frame.width;
	data->g_frameSize.height = params->frame.height;
	//��ʼ����ʦ��
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
	//��ʼ���������
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
	//��ʼ����ֵ
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

	//��ʼ��������ɫ
	data->pink_colour = colour_RGB2YUV(255, 0, 255);	/*�ۺ�*/
	data->blue_colour = colour_RGB2YUV(0, 0, 255);		/*����*/
	data->lilac_colour = colour_RGB2YUV(155, 155, 255);/*����*/
	data->green_colour = colour_RGB2YUV(0, 255, 0);	/*����*/
	data->red_colour = colour_RGB2YUV(255, 0, 0);		/*����*/
	data->dullred_colour = colour_RGB2YUV(127, 0, 0);	/*����*/
	data->yellow_colour = colour_RGB2YUV(255, 255, 0);	/*����*/
	data->black_colour = colour_RGB2YUV(0, 0, 0);
	return 0;
}
