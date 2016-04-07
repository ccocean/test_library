#ifndef _ITC_DECODE_H_
#define _ITC_DECODE_H_

extern "C"
{
#include "include/libavcodec/avcodec.h"  
#include "include/libavformat/avformat.h"
#include "include/libavutil/avutil.h"  
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
}

class myDecoder_t
{
public:
	myDecoder_t();
	~myDecoder_t();
	bool decoder(unsigned char *inputbuffer, int frame_size, int height, int width, unsigned char *buffer);
	bool init(AVCodecID typeID);
	void release();
protected:
	AVCodec         *pCodec;
	AVCodecContext  *pCodecCtx;
	AVCodecParserContext *pCodecParserCtx;

	AVFrame         *pFrame;
	AVPacket        packet;
	bool			m_init_flag;
};

#endif