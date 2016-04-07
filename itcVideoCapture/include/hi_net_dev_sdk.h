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

#define HI_NET_DEV_STREAM_1 			0 /* ������ */
#define HI_NET_DEV_STREAM_2 			1 /* ������ */

/* �豸ͨ���ţ��������֧��һ��ͨ�� */
#define HI_NET_DEV_CHANNEL_1 1

/* ������������ģʽ��Ŀǰ��֧�� TCP */
#define HI_NET_DEV_STREAM_MODE_TCP 0

/* ���������� */
#define HI_NET_DEV_STREAM_VIDEO_ONLY 	0x01
#define HI_NET_DEV_STREAM_AUDIO_ONLY 	0x02
#define HI_NET_DEV_STREAM_VIDEO_AUDIO 	0x03
#define HI_NET_DEV_STREAM_YUV_ONLY		0x04
#define HI_NET_DEV_STREAM_VIDEO_YUV		0x05
#define HI_NET_DEV_STREAM_ALL           0x07

/* �����ý�����ݻص� */
#define HI_NET_DEV_AV_DATA				0	/* ����Ƶ���� */
#define HI_NET_DEV_SYS_DATA				1	/* ϵͳ���� */
#define HI_NET_DEV_VIDEO_FRAME_I		1	/* �ؼ�֡ */
#define HI_NET_DEV_VIDEO_FRAME_P		2	/* �ǹؼ�֡ */

/* ͼ�������� */
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

///��Ƶ��ʽ
#define HI_NET_DEV_AUDIO_TYPE_AAC		100		/* AAC */

///��������
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

///��̨���ƶ���
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

#define HI_NET_DEV_VIDEO_FRAME_FLAG 		0x46565848	/* ��Ƶ֡��־ */
#define HI_NET_DEV_AUDIO_FRAME_FLAG 		0x46415848  /* ��Ƶ֡��־ */
#define HI_NET_DEV_VIDEO_YUV_FLAG 			0x56595556  /* YUV֡��־ */

#define HI_NET_DEV_CONNECTED				1		/* �Ѿ����� */
#define HI_NET_DEV_CONNECT_FAILED			2		/* ����ʧ�� */ 
#define HI_NET_DEV_ABORTIBE_DISCONNECTED 	3 		/* �ر����� */

#define HI_SDK_AUDIO_ENCODETYPE_AAC  	0xaac
#define HI_SDK_AUDIO_SAMPLERATE_44100 	44100	
#define HI_SDK_AUDIO_SAMPLERATE_48000 	48000	
#define HI_SDK_AUDIO_BITWIDTH_16		16	
#define HI_SDK_AUDIO_BITWIDTH_32		32	
#define HI_SDK_AUDIO_SOUNDMODE_MONO		1
#define HI_SDK_AUDIO_SOUNDMODE_STEREO	2

typedef struct 
{
	HI_U32 u32AVFrameFlag;   /* ֡��־ */
	HI_U32 u32AVFrameLen;    /* ֡�ĳ��� */
	HI_U32 u32AVFramePTS;    /* ʱ��� */
	HI_U32 u32VFrameType;    /* ��Ƶ�����ͣ�I֡��P֡ */
} HI_S_AVFrame;

typedef struct 
{
	HI_U32 u32Flag;   		/* ֡��־ */
	HI_U32 u32Len;    		/* ֡�ĳ��� */
	HI_U32 u32Width;  		/* ͼ���� */
	HI_U32 u32Height; 		/* ͼ��߶� */
	HI_U32 u32Stride[3]; 	/* ͼ���� */
	HI_U64 u64PTS;    		/* ʱ�� */
	HI_U32 u32Ref;    		/* ͼ��֡���к� */
} HI_S_YUVFrame;

/* ϵͳͷ */
#define HI_NET_DEV_SYS_FLAG 0x53595346

typedef struct
{
	HI_U32 u32Width;		/* ��Ƶ�� */
	HI_U32 u32Height;    	/* ��Ƶ�� */
} HI_S_VideoHeader;

typedef struct
{
	HI_U32 u32Format;			/* ��Ƶ��ʽ AAC */
} HI_S_AudioHeader;

typedef struct
{
	HI_U32 u32Stream;           /* �������� */
	HI_U32 u32FrameRate;        /* ֡�� */
	HI_U32 u32BitRate;          /* ���� */
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
	HI_U32 u32Channel; 	/* ͨ���ţ����û�ȡ�������Ӧ */
	HI_U32 u32Stream; 	/* 1:������������2:���Ӵ����� */
	HI_U32 u32Mode; 	/* ��������ģʽ */
	HI_U8 u8Type; 		/* ���������ͣ���Ƶ����Ƶ���������� */
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
	HI_U32 u32Channel;  	/* ͨ�� */
	HI_U32 u32Stream;   	/* ����������  */
	HI_U32 u32Bitrate;		/* ���� */
	HI_U32 u32Frame;    	/* ֡�� */
	HI_U32 u32Iframe;		/* ��֡���1-300 */
	HI_U32 u32Cbr;	    	/* ����ģʽ��V_TRUEΪ�̶�������V_FALSEΪ�ɱ����� */		
	HI_U32 u32ImgQuality;	/* ��������1-6 */
	HI_U32 u32Resolution;	/* �ֱ��� */
	HI_U32 u32Width;  		/* ��Ƶ�ֱ��ʱ��룬���û����ƴ�С�Ŀ�� */
	HI_U32 u32Height;    	/* �û����ƴ�С�ĸ߶� */
} HI_S_Video_Ext;

/* ͼ��������� */
typedef struct HI_Display_Ext
{
	HI_BOOL blFlip;         /* ���·�ת */
	HI_BOOL blMirror;     	/* ���ҷ�ת*/
	HI_S32 s32Scene;
} HI_S_Display_Ext;

/* ͼ��������� */
typedef struct HI_Display
{
	HI_U32 u32Brightness;   /*  ����  	*/
	HI_U32 u32Saturation;   /*  ���Ͷ�  */
	HI_U32 u32Contrast;     /*  �Աȶ�  */
	HI_U32 u32Hue;          /*  ɫ��  	*/
} HI_S_Display;

/* OSD �������� */
#if 0
typedef struct HI_OSD
{
	HI_BOOL blEnTime; 		/* ����ʱ�� */
	HI_BOOL blEnName; 		/* �������� */
	HI_CHAR sName[64]; 		/* OSD ����, ��� 18 �ֽ� */
} HI_S_OSD;
#endif

/* time �������� */
typedef struct hiSERVERTIME_INFO_S
{
	HI_CHAR sTime[32]; 		/* �����ʱ�䣬��ʽ 201103110912.08 */
} HI_S_SERVERTIME;

/* audio �������� */
typedef struct HI_Audio_Ext
{
	HI_U32 u32Channel; 		/* ͨ�� */
	HI_U32 u32Stream; 		/* ������ */
	HI_BOOL blEnable; 		/* �Ƿ�ɼ���Ƶ */
	HI_U32 u32Type; 		/* ��Ƶ��ʽ */
	HI_U32 u32SampleRate;	/* ������ */
	HI_U32 u32BitRate;		/* ���� */
	HI_U32 s32AudioVolume;	/* ���� */
} HI_S_Audio_Ext;

/* audio volume �������� */
typedef struct HI_AudioVolume
{
	HI_S32 s32AudioVolume; /* ��Ƶ��������Χ�� (-97db)-(30db) */
} HI_S_AudioVolume;

/* �ֱ������� */
typedef struct HI_Resolution 
{ 
	HI_U32 u32Channel;		//ͨ��
	HI_U32 u32Stream;		//����������־��0 ��������1 ������
	HI_U32 u32Resolution;	//������
} HI_S_Resolution; 

/* �������� */
typedef struct HI_OSD_TEXT
{
	HI_BOOL  blEnable;        /* ʹ��     */
	HI_U32   x;               /* 0 - 100  */
	HI_U32   y;               /* 0 - 100  */
	HI_CHAR  sName[32];       /* ��ʾ���� */
}HI_S_OSD_TEXT;

typedef struct HI_OSD_DATE
{
	HI_BOOL  blEnable;       /* ʹ�� */
	HI_U32   u32Mode;        /* ʱ����ʾ��ʽ */
	HI_U32   x;              /* 0 - 100 */
	HI_U32   y;              /* 0 - 100 */
}HI_S_OSD_DATE;

typedef struct HI_OSD
{
	HI_U32 u32Channel;			 /* 0 ������  1 ������ */
	HI_S_OSD_TEXT struText;   	 /* ���������������� */
	HI_S_OSD_DATE struDate;     /* ������������ʱ�� */
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

