#ifndef _HI_NET_DEV_API_H_
#define _HI_NET_DEV_API_H_

#ifndef WIN32
#define WIN32
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "hi_type.h"
#include "hi_net_dev_errors.h"

#define HI_NET_DEV_PARAM_ERROR          0x41001
#define HI_NET_DEV_MEMORY_ERROR         0x41002
#define HI_NET_DEV_NOT_SUPPORT          0x41003
#define HI_NET_DEV_PARAM_CHECK_ERROR	0x51001		//Paramter input error
#define HI_NET_DEV_PARAM_CMD_ERROR		0x51002		//No command
#define HI_NET_DEV_PARAM_PARSE_ERROR	0x51003		//parsse command 
#define HI_NET_DEV_NET_CONNECT_FAIL		0x52001		//connect host failure
#define HI_NET_DEV_NET_TRANSFER_FAIL	0x52002		//transfer host failure
#define HI_NET_DEV_NET_RETURN_ERROR		0x52003		//host return error
#define HI_NET_DEV_NET_NOT_SUPPORT		0x53000		//device not support the paramter

#define HI_NET_DEV_STREAM_1 			0 /* 主码流 */
#define HI_NET_DEV_STREAM_2 			1 /* 次码流 */

/* 设备通道号，摄像机仅支持一个通道 */
#define HI_NET_DEV_CHANNEL_1 1

/* 连接网络连接模式，目前仅支持 TCP */
#define HI_NET_DEV_STREAM_MODE_TCP 0

/* 流数据类型 */
#define HI_NET_DEV_STREAM_VIDEO_ONLY 	0x01
#define HI_NET_DEV_STREAM_AUDIO_ONLY 	0x02
#define HI_NET_DEV_STREAM_VIDEO_AUDIO 	0x03
#define HI_NET_DEV_STREAM_YUV_ONLY		0x04
#define HI_NET_DEV_STREAM_VIDEO_YUV		0x05
#define HI_NET_DEV_STREAM_ALL           0x07

/* 网络多媒体数据回调 */
#define HI_NET_DEV_AV_DATA				0	/* 音视频数据 */
#define HI_NET_DEV_SYS_DATA				1	/* 系统数据 */
#define HI_NET_DEV_VIDEO_FRAME_I		1	/* 关键帧 */
#define HI_NET_DEV_VIDEO_FRAME_P		2	/* 非关键帧 */

/* 图像清晰度 */
#define HI_NET_DEV_RESOLUTION_VGA		0
#define HI_NET_DEV_RESOLUTION_QVGA		1
#define HI_NET_DEV_RESOLUTION_QQVGA 	2
#define HI_NET_DEV_RESOLUTION_D1		3
#define HI_NET_DEV_RESOLUTION_CIF		4
#define HI_NET_DEV_RESOLUTION_QCIF		5
#define HI_NET_DEV_RESOLUTION_720P		6
#define HI_NET_DEV_RESOLUTION_Q720		7
#define HI_NET_DEV_RESOLUTION_QQ720		8
#define HI_NET_DEV_RESOLUTION_UXGA		9
#define HI_NET_DEV_RESOLUTION_960H		10
#define HI_NET_DEV_RESOLUTION_Q960H		11
#define HI_NET_DEV_RESOLUTION_QQ960H	12
#define HI_NET_DEV_RESOLUTION_1080P		13
#define HI_NET_DEV_RESOLUTION_960P		14
#define HI_NET_DEV_RESOLUTION_576P		15
#define HI_NET_DEV_RESOLUTION_USER      16

///音频格式
#define HI_NET_DEV_AUDIO_TYPE_AAC		100		/* AAC */

///参数定义
#define HI_NET_DEV_CMD_DISPLAY				0x1001
#define HI_NET_DEV_CMD_DISPLAY_EXT          0x1002
#define HI_NET_DEV_CMD_OSD_PARAM            0x1005
#define HI_NET_DEV_CMD_RESOLUTION			0x1008
#define HI_NET_DEV_CMD_SERVER_TIME          0x1017
#define HI_NET_DEV_CMD_REBOOT               0x1018
#define HI_NET_DEV_CMD_RESET                0x1019
#define HI_NET_DEV_CMD_VIDEO_PARAM_EXT      0x1047
#define HI_NET_DEV_CMD_AUDIO_PARAM_EXT      0x1048
#define HI_NET_DEV_CMD_AUDIO_VOLUME_IN      0x1070

///云台控制定义
#define HI_NET_DEV_CTRL_PTZ_STOP            0x3000
#define HI_NET_DEV_CTRL_PTZ_UP              0x3001
#define HI_NET_DEV_CTRL_PTZ_DOWN            0x3002
#define HI_NET_DEV_CTRL_PTZ_LEFT            0x3003
#define HI_NET_DEV_CTRL_PTZ_RIGHT           0x3004
#define HI_NET_DEV_CTRL_PTZ_ZOOMIN          0x3005
#define HI_NET_DEV_CTRL_PTZ_ZOOMOUT         0x3006
#define HI_NET_DEV_CTRL_PTZ_FOCUSIN         0x3007
#define HI_NET_DEV_CTRL_PTZ_FOCUSOUT        0x3008
#define HI_NET_DEV_CTRL_PTZ_ZOOMSTOP        0x3011
#define HI_NET_DEV_CTRL_PTZ_FOCUSSTOP       0x3012
#define HI_NET_DEV_CTRL_PTZ_GOTO_PRESET     0x3015
#define HI_NET_DEV_CTRL_PTZ_SET_PRESET      0x3016
#define HI_NET_DEV_CTRL_PTZ_CLE_PRESET      0x3017
#define HI_NET_DEV_CTRL_PTZ_HOME            0x3027

//New Add
#define HI_NET_DEV_CTRL_PTZ_LEFT_UP         0x3040
#define HI_NET_DEV_CTRL_PTZ_LEFT_DOWN       0x3041
#define HI_NET_DEV_CTRL_PTZ_RIGHT_UP        0x3042
#define HI_NET_DEV_CTRL_PTZ_RIGHT_DOWN      0x3043

#define HI_NET_DEV_CTRL_PTZ_FOCUS_AUTO      0x3044
#define HI_NET_DEV_CTRL_PTZ_FOCUS_MANUAL    0x3045

#define HI_NET_DEV_CTRL_PTZ_ZOOM_QUERY      0x3050
#define HI_NET_DEV_CTRL_PTZ_FOCUS_QUERY     0x3051
#define HI_NET_DEV_CTRL_PTZ_PAN_TILE_QUERY  0x3052

#define HI_NET_DEV_CTRL_PTZ_ABS_POSITION    0x3060
#define HI_NET_DEV_CTRL_PTZ_REL_POSITION    0x3061

#define HI_NET_DEV_CTRL_PTZ_PRESET_MAX		254
#define HI_NET_DEV_CTRL_PTZ_PRESET_MIN      0

#define HI_NET_DEV_VIDEO_FRAME_FLAG 		0x46565848	/* 视频帧标志 */
#define HI_NET_DEV_AUDIO_FRAME_FLAG 		0x46415848  /* 音频帧标志 */
#define HI_NET_DEV_VIDEO_YUV_FLAG 			0x56595556  /* YUV帧标志 */

#define HI_NET_DEV_CONNECTED				1		/* 已经连接 */
#define HI_NET_DEV_CONNECT_FAILED			2		/* 连接失败 */ 
#define HI_NET_DEV_ABORTIBE_DISCONNECTED 	3 		/* 关闭连接 */

#define HI_SDK_AUDIO_ENCODETYPE_AAC  	0xaac
#define HI_SDK_AUDIO_SAMPLERATE_44100 	44100	
#define HI_SDK_AUDIO_SAMPLERATE_48000 	48000	
#define HI_SDK_AUDIO_BITWIDTH_16		16	
#define HI_SDK_AUDIO_BITWIDTH_32		32	
#define HI_SDK_AUDIO_SOUNDMODE_MONO		1
#define HI_SDK_AUDIO_SOUNDMODE_STEREO	2

typedef struct 
{
	HI_U32 u32AVFrameFlag;   /* 帧标志 */
	HI_U32 u32AVFrameLen;    /* 帧的长度 */
	HI_U32 u32AVFramePTS;    /* 时间戳 */
	HI_U32 u32VFrameType;    /* 视频的类型，I帧或P帧 */
} HI_S_AVFrame;

typedef struct 
{
	HI_U32 u32Flag;   		/* 帧标志 */
	HI_U32 u32Len;    		/* 帧的长度 */
	HI_U32 u32Width;  		/* 图像宽度 */
	HI_U32 u32Height; 		/* 图像高度 */
	HI_U32 u32Stride[3]; 	/* 图像跨距 */
	HI_U64 u64PTS;    		/* 时戳 */
	HI_U32 u32Ref;    		/* 图像帧序列号 */
} HI_S_YUVFrame;

/* 系统头 */
#define HI_NET_DEV_SYS_FLAG 0x53595346

typedef struct
{
	HI_U32 u32Width;		/* 视频宽 */
	HI_U32 u32Height;    	/* 视频高 */
} HI_S_VideoHeader;

typedef struct
{
	HI_U32 u32Format;			/* 音频格式 AAC */
} HI_S_AudioHeader;

typedef struct
{
	HI_U32 u32Stream;           /* 主辅码流 */
	HI_U32 u32FrameRate;        /* 帧率 */
	HI_U32 u32BitRate;          /* 码率 */
} HI_S_StreamHeader;

typedef struct 
{
	HI_U32 u32SysFlag;
	HI_S_VideoHeader struVHeader;
	HI_S_AudioHeader struAHeader;
	HI_S_StreamHeader struSHeader;
} HI_S_SysHeader;

typedef struct
{
	HI_U32 u32Channel; 	/* 通道号，设置获取属性相对应 */
	HI_U32 u32Stream; 	/* 1:连接主码流，2:连接次码流 */
	HI_U32 u32Mode; 	/* 网络连接模式 */
	HI_U8 u8Type; 		/* 流数据类型，视频，音频，其他数据 */
} HI_S_STREAM_INFO_EXT;

typedef HI_S32 (*HI_ON_STREAM_CALLBACK)(
	HI_U32 u32Handle,
	HI_U32 u32DataType,
	HI_U8* pu8Buffer,
	HI_U32 u32Length,
	HI_VOID* pUserData
);

typedef HI_S32 (*HI_ON_EVENT_CALLBACK) (
	HI_U32 u32Handle,
	HI_U32 u32Event,
	HI_VOID* pUserData
);

typedef struct HI_Video_Ext
{
	HI_U32 u32Channel;  	/* 通道 */
	HI_U32 u32Stream;   	/* 主、次码流  */
	HI_U32 u32Bitrate;		/* 码流 */
	HI_U32 u32Frame;    	/* 帧率 */
	HI_U32 u32Iframe;		/* 主帧间隔1-300 */
	HI_U32 u32Cbr;	    	/* 码流模式、V_TRUE为固定码流，V_FALSE为可变码流 */		
	HI_U32 u32ImgQuality;	/* 编码质量1-6 */
	HI_U32 u32Resolution;	/* 分辨率 */
	HI_U32 u32Width;  		/* 视频分辨率编码，或用户定制大小的宽度 */
	HI_U32 u32Height;    	/* 用户定制大小的高度 */
} HI_S_Video_Ext;

/* 图像参数设置 */
typedef struct HI_Display_Ext
{
	HI_BOOL blFlip;         /* 上下翻转 */
	HI_BOOL blMirror;     	/* 左右翻转*/
	HI_S32 s32Scene;
} HI_S_Display_Ext;

/* 图像参数设置 */
typedef struct HI_Display
{
	HI_U32 u32Brightness;   /*  亮度  	*/
	HI_U32 u32Saturation;   /*  饱和度  */
	HI_U32 u32Contrast;     /*  对比度  */
	HI_U32 u32Hue;          /*  色度  	*/
} HI_S_Display;

/* OSD 参数设置 */
#if 0
typedef struct HI_OSD
{
	HI_BOOL blEnTime; 		/* 叠加时间 */
	HI_BOOL blEnName; 		/* 叠加名称 */
	HI_CHAR sName[64]; 		/* OSD 名称, 最大 18 字节 */
} HI_S_OSD;
#endif

/* time 参数设置 */
typedef struct hiSERVERTIME_INFO_S
{
	HI_CHAR sTime[32]; 		/* 摄像机时间，格式 201103110912.08 */
} HI_S_SERVERTIME;

/* audio 参数设置 */
typedef struct HI_Audio_Ext
{
	HI_U32 u32Channel; 		/* 通道 */
	HI_U32 u32Stream; 		/* 码流号 */
	HI_BOOL blEnable; 		/* 是否采集音频 */
	HI_U32 u32Type; 		/* 音频格式 */
	HI_U32 u32SampleRate;	/* 采样率 */
	HI_U32 u32BitRate;		/* 码率 */
	HI_U32 s32AudioVolume;	/* 音量 */
} HI_S_Audio_Ext;

/* audio volume 参数设置 */
typedef struct HI_AudioVolume
{
	HI_S32 s32AudioVolume; /* 音频音量，范围： (-97db)-(30db) */
} HI_S_AudioVolume;

/* 分辨率设置 */
typedef struct HI_Resolution 
{ 
	HI_U32 u32Channel;		//通道
	HI_U32 u32Stream;		//主次码流标志，0 主码流，1 次码流
	HI_U32 u32Resolution;	//清晰度
} HI_S_Resolution; 

/* 叠加设置 */
typedef struct HI_OSD_TEXT
{
	HI_BOOL  blEnable;        /* 使能     */
	HI_U32   x;               /* 0 - 100  */
	HI_U32   y;               /* 0 - 100  */
	HI_CHAR  sName[32];       /* 显示内容 */
}HI_S_OSD_TEXT;

typedef struct HI_OSD_DATE
{
	HI_BOOL  blEnable;       /* 使能 */
	HI_U32   u32Mode;        /* 时间显示方式 */
	HI_U32   x;              /* 0 - 100 */
	HI_U32   y;              /* 0 - 100 */
}HI_S_OSD_DATE;

typedef struct HI_OSD
{
	HI_U32 u32Channel;			 /* 0 主码流  1 辅码流 */
	HI_S_OSD_TEXT struText;   	 /* 主辅码流叠加名称 */
	HI_S_OSD_DATE struDate;     /* 主辅码流叠加时间 */
} HI_S_OSD;

#ifdef WIN32
#ifdef NETLIB_DLL_EXPORTS
#define NET_API      __declspec( dllexport )
#define NET_APICALL  __stdcall
#else
#define NET_API
#define NET_APICALL
#endif
#endif

NET_API HI_S32  NET_APICALL HI_NET_DEV_Init( void );

NET_API HI_S32  NET_APICALL HI_NET_DEV_DeInit( void );

NET_API HI_S32 NET_APICALL HI_NET_DEV_Login(
		HI_U32* pu32Handle, 
		const HI_CHAR* psUsername, 
		const HI_CHAR* psPassword, 
		const HI_CHAR* psHost, 
		HI_U16 u16Port);

NET_API HI_S32 NET_APICALL HI_NET_DEV_Logout( 
		HI_U32 u32Handle); 

NET_API HI_S32 NET_APICALL HI_NET_DEV_SetReconnect ( 
		HI_U32 u32Handle,
		HI_U32 u32Interval);

NET_API HI_S32 NET_APICALL HI_NET_DEV_StartStreamExt ( 
		HI_U32 u32Handle, 
		HI_S_STREAM_INFO_EXT* pstruStreamInfo);

NET_API HI_S32 NET_APICALL HI_NET_DEV_StopStream( 
		HI_U32 u32Handle);

NET_API HI_S32 NET_APICALL HI_NET_DEV_SetStreamCallBack ( 
		HI_U32 u32Handle,
		HI_ON_STREAM_CALLBACK cbStreamCallBack, 
		HI_VOID* pUserData);

NET_API HI_S32 NET_APICALL HI_NET_DEV_SetEventCallBack(
		HI_U32 u32Handle,
		HI_ON_EVENT_CALLBACK cbEventCallBack,
		HI_VOID* pUserData);

NET_API HI_S32 NET_APICALL HI_NET_DEV_MakeKeyFrame ( 
		HI_U32 u32Handle, 
		HI_U32 u32Stream );

NET_API HI_S32 NET_APICALL HI_NET_DEV_GetConfig ( 
		HI_U32 u32Handle,
		HI_U32 u32Command, 
		HI_VOID* pBuf, 
		HI_U32 u32BufLen);

NET_API HI_S32 NET_APICALL HI_NET_DEV_SetConfig( 
		HI_U32 u32Handle,
		HI_U32 u32Command, 
		HI_VOID* pBuf, 
		HI_U32 u32BufLen);

NET_API HI_S32 NET_APICALL HI_NET_DEV_PTZ_Ctrl_Standard ( 
		HI_U32 u32Handle,
		HI_U32 u32Command, 
		HI_U32 u32Speed);

NET_API HI_S32 NET_APICALL HI_NET_DEV_PTZ_Ctrl_Preset ( 
		HI_U32 u32Handle,
		HI_U32 u32Command, 
		HI_U32 u32Preset );

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _HI_NET_DEV_API_H_ */

