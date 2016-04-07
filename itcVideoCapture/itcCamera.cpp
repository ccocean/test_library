#include "itcCamera.h"

camera::camera()
{
	m_bConnectFlag = false;
	m_bStreamFlag = false;
	HI_NET_DEV_Init();
}

camera::~camera()
{
	if (m_bConnectFlag)
	{
		StreamStop();
		logout();
	}
}



bool camera::login(unsigned int* pu32Handle, std::string psUsername, std::string psPassword, unsigned long dIP, int u16Port)
{
	if (m_bConnectFlag)
		return true;

	HI_S32 s32Ret = HI_SUCCESS;
	std::string Addr;
	std::ostringstream  ostr;
	ostr << ((dIP >> 24) & 255) << "." << ((dIP >> 16) & 255) << "." << ((dIP >> 8) & 255) << "." << (dIP & 255);
	Addr = ostr.str();
	s32Ret = HI_NET_DEV_Login(pu32Handle, psUsername.c_str(), psPassword.c_str(), Addr.c_str(), u16Port);
	m_handle = *pu32Handle;
	HI_NET_DEV_SetReconnect(m_handle, 3);
	if (HI_SUCCESS == s32Ret)
	{
		m_bConnectFlag = true;
		//SetConfigDefault();
		return true;
	}
	else
	{
		m_bConnectFlag = false;
		return false;
	}
}

bool camera::logout()
{
	if (m_bConnectFlag == true)
	{
		if (HI_NET_DEV_Logout(m_handle) == HI_SUCCESS)
		{
			m_bConnectFlag = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool camera::StreamStart()
{
	if (m_bConnectFlag)
	{
		if (!m_bStreamFlag)
		{
			HI_S32 s32Ret = HI_SUCCESS;
			HI_S_STREAM_INFO_EXT struStreamInfo;
			m_decoder.init(AV_CODEC_ID_H264);
			HI_NET_DEV_SetStreamCallBack(m_handle, OnStreamCallback, (HI_VOID*)this);
			struStreamInfo.u32Stream = HI_NET_DEV_STREAM_1;
			struStreamInfo.u32Channel = HI_NET_DEV_CHANNEL_1;
			struStreamInfo.u32Mode = HI_NET_DEV_STREAM_MODE_TCP;
			struStreamInfo.u8Type = HI_NET_DEV_STREAM_ALL;
			s32Ret = HI_NET_DEV_StartStreamExt(m_handle, &struStreamInfo);
			if (s32Ret == HI_SUCCESS)
			{
				m_bStreamFlag = true;
				return true;
			}
		}
	}
	return false;
}

bool camera::StreamStop()
{
	HI_S32 s32Ret = HI_SUCCESS;

	if (m_bConnectFlag)
	{
		if (m_bStreamFlag)
		{
			s32Ret = HI_NET_DEV_StopStream(m_handle);
			if (s32Ret == HI_SUCCESS)
			{
				m_bStreamFlag = false;
				return true;
			}
			
		}
	}
	return false;
}

cv::Mat camera::getImageBuffer()
{
	cv::Mat temp;
	m_lock_streamBuffer.Lock();
	if (m_stream_buffer.size() > 0)
	{
		temp = m_stream_buffer.front();
		m_stream_buffer.pop_front();

	}
	else
	{
		temp=cv::Mat();
	}
	m_lock_streamBuffer.Unlock();
	return temp;
}

bool camera::SetConfigDefault()
{
	HI_S_Video_Ext sVideo;
	sVideo.u32Channel = HI_NET_DEV_CHANNEL_1;
	sVideo.u32Stream = HI_NET_DEV_STREAM_1;	// 主码流
	sVideo.u32Resolution = HI_NET_DEV_RESOLUTION_1080P;
	sVideo.u32Bitrate = 4096;
	sVideo.u32Frame = 1;
	sVideo.u32Iframe = 30;
	sVideo.u32Cbr = HI_TRUE;// 固定码率
	sVideo.u32ImgQuality = 1;
	sVideo.u32Width = 1920;
	sVideo.u32Height = 1080;
	if (HI_NET_DEV_SetConfig(m_handle, HI_NET_DEV_CMD_VIDEO_PARAM_EXT, &sVideo, sizeof(HI_S_Video_Ext)) == HI_SUCCESS)
	{
		return true;
	}
	return false;
}

void SaveRecordFile(HI_CHAR* pPath, HI_U8* pu8Buffer, HI_U32 u32Length)
{
	FILE* fp;
	fopen_s(&fp, pPath, "ab+");
	fwrite(pu8Buffer, 1, u32Length, fp);
	fclose(fp);
}

//数据流回调函数
static HI_S32 OnStreamCallback(HI_U32 u32Handle,					/* 句柄*/
	HI_U32 u32DataType,			/* 数据类型，视频或音频数据或音视频复合数据*/
	HI_U8*  pu8Buffer,			/* 数据包含帧头 */
	HI_U32 u32Length,			/* 数据长度*/
	HI_VOID* pUserData)			/* 用户数据*/
{
	HI_S_AVFrame* pstruAV = HI_NULL;
	HI_S_SysHeader* pstruSys = HI_NULL;

	camera *pCamera = (camera*)pUserData;

	if (u32DataType == HI_NET_DEV_AV_DATA)
	{
		pstruAV = (HI_S_AVFrame*)pu8Buffer;//u32VFrameType==1是关键帧I
		if (pstruAV->u32AVFrameFlag == HI_NET_DEV_VIDEO_FRAME_FLAG)
		{
			//视频数据
			//SaveRecordFile("D://video1.h264", pu8Buffer + sizeof(HI_S_AVFrame), u32Length - sizeof(HI_S_AVFrame));
			int sizeData = u32Length - sizeof(HI_S_AVFrame);
			unsigned char *dataBuffer = pu8Buffer + sizeof(HI_S_AVFrame);
			cv::Mat m_imageBuffer = cv::Mat(pCamera->m_height*3/2, pCamera->m_width, CV_8UC1);
			pCamera->m_decoder.decoder(dataBuffer, sizeData, pCamera->m_height, pCamera->m_width, m_imageBuffer.data);

			(pCamera->m_lock_streamBuffer).Lock();
			if (pCamera->m_stream_buffer.size()<15)
			{
				(pCamera->m_stream_buffer).push_back(m_imageBuffer);
			}
			else
			{
				(pCamera->m_stream_buffer).pop_front();
			}
			(pCamera->m_lock_streamBuffer).Unlock();

		}
		else if (pstruAV->u32AVFrameFlag == HI_NET_DEV_AUDIO_FRAME_FLAG)
		{
			//音频数据
		}
	}
	else if (u32DataType == HI_NET_DEV_SYS_DATA)
	{
		pstruSys = (HI_S_SysHeader*)pu8Buffer;//返回的是系统参数
		pCamera->m_height = pstruSys->struVHeader.u32Height;
		pCamera->m_width = pstruSys->struVHeader.u32Width;

	}
	return HI_SUCCESS;
}