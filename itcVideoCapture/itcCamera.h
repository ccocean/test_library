#ifndef itcCamera_h__
#define itcCamera_h__

#include <string>
#include <sstream>
#include <deque>
#include "opencv2/opencv.hpp"
#include "decoder.h"
#include "CMyLock.h"

#include "include/hi_net_dev_sdk.h"
#pragma comment(lib,"NetLib.lib")

class camera
{
public:
	camera();
	~camera();

	bool login(unsigned int* pu32Handle, std::string psUsername, std::string psPassword, unsigned long dIP, int u16Port);
	
	bool logout();

	int m_height;
	int m_width;
	Mutex m_lock_streamBuffer;
	myDecoder_t m_decoder;
	std::deque<cv::Mat> m_stream_buffer;
	bool StreamStart();
	bool StreamStop();
	cv::Mat getImageBuffer();

	bool SetConfigDefault();
protected:
	HI_U32 m_handle;
	bool m_bConnectFlag;
	bool m_bStreamFlag;

	
private:
};

//数据流回调函数
static HI_S32 OnStreamCallback(HI_U32 u32Handle,					/* 句柄*/
	HI_U32 u32DataType,			/* 数据类型，视频或音频数据或音视频复合数据*/
	HI_U8*  pu8Buffer,			/* 数据包含帧头 */
	HI_U32 u32Length,			/* 数据长度*/
	HI_VOID* pUserData);
#endif