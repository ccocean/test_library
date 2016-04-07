#include "decoder.h"

myDecoder_t::myDecoder_t()
{
	pCodec = NULL;
	pCodecCtx = NULL;
	pCodecParserCtx = NULL;

	pFrame = NULL;
	m_init_flag = false;
}

myDecoder_t::~myDecoder_t()
{
	release();
}

bool myDecoder_t::init(AVCodecID typeID)
{
	if (m_init_flag == false)
	{	
		release();
	}

	avcodec_register_all();

	/* find the h264 video decoder */
	pCodec = avcodec_find_decoder(typeID);
	if (!pCodec) {
		//MessageBox(NULL, TEXT("解码器未找到。"), TEXT("标题"), MB_OK);
		return false;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (!pCodecCtx)
	{
		//MessageBox(NULL, TEXT("无法分配视频解码器上下文。"), TEXT("标题"), MB_OK);
		return false;
	}

	pCodecParserCtx = av_parser_init(typeID);
	if (!pCodecParserCtx){
		//MessageBox(NULL, TEXT("无法分配视频分割器上下文。"), TEXT("标题"), MB_OK);
		return false;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		//MessageBox(NULL, TEXT("无法打开解码器。"), TEXT("标题"), MB_OK);
		return false;
	}

	pFrame = av_frame_alloc();
	if (pFrame == NULL)
	{
		return false;
	}

	av_init_packet(&packet);
	m_init_flag = true;
	return true;
}

void myDecoder_t::release()
{
	if (pFrame != NULL)
	{
		av_frame_free(&pFrame);
		pFrame = NULL;
	}
	if (pCodecCtx != NULL)
	{
		avcodec_free_context(&pCodecCtx);
		pCodecCtx = NULL;
	}
	pCodec = NULL;
	pCodecParserCtx = NULL;

	m_init_flag = false;
}

bool myDecoder_t::decoder(unsigned char *inputbuffer, int frame_size, int height, int width, unsigned char *buffer)
{
	if (m_init_flag == false)
	{
		return false;
	}

	if (buffer==NULL)
	{
		//MessageBox(NULL, TEXT("图像缓冲区未分配"), TEXT("标题"), MB_OK);
		return false;
	}
	int got_picture = 0;
	int av_result = 0;
	packet.data = inputbuffer;
	packet.size = frame_size;
	av_result = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
	if (av_result < 0)
	{
		return false;
	}
	if (got_picture)
	{
		int size = height;
		unsigned char* data = buffer;
		unsigned char* srcdata = pFrame->data[0];
		int i = 0;
		for (i = 0; i < size; i++)
		{	
			memcpy(data, srcdata, width);	//Y
			data += width;
			srcdata += pFrame->width;
		}
		size /= 4;
		srcdata = pFrame->data[1];
		for (i = 0; i < size; i++)
		{
			memcpy(data, srcdata, width);	//U
			data += width;
			srcdata += pFrame->width;
		}
		srcdata = pFrame->data[2];
		for (i = 0; i < size; i++)
		{
			memcpy(data, srcdata, width);	//V
			data += width;
			srcdata += pFrame->width;
		}
		return true;
	}
	else
	{
		return false;
	}

}

