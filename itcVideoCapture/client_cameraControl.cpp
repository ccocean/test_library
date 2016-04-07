#include "client_cameraControl.h"

static DWORD WINAPI  recv_thread(LPVOID argv)
{
	PanAndTiltCameraControl* p = (PanAndTiltCameraControl*)argv;
	int len;
	char buffer[256];
	int posit_pan;
	int posit_tilt;
	int zoomValue;
	while (p->getRun_Status() == TRUE)
	{
		if (p->getStart_Status() == TRUE)
		{	
			len = p->recv_CameraInfo(buffer);
			if (len >= 7)
			{
				if (buffer[1] = 0x50)
				{
					if (len == 11)
					{
						posit_pan = ((buffer[2] & 0x0f) << 12) + ((buffer[3] & 0x0f) << 8) + ((buffer[4] & 0x0f) << 4) + (buffer[5] & 0x0f);
						posit_pan = ((posit_pan >> 15) & 1) ? posit_pan | ((-1 >> 16) << 16) : posit_pan & 0xffff;

						posit_tilt = ((buffer[6] & 0x0f) << 12) + ((buffer[7] & 0x0f) << 8) + ((buffer[8] & 0x0f) << 4) + (buffer[9] & 0x0f);
						posit_tilt = ((posit_tilt >> 15) & 1) ? posit_tilt | ((-1 >> 16) << 16) : posit_tilt & 0xffff;
						p->set_CameraInfo_panTilt(posit_pan, posit_tilt);
						::SetEvent(p->m_hHandle1);
					}
					else if (len == 7)
					{
						zoomValue = ((buffer[2] & 0x0f) << 12) + ((buffer[3] & 0x0f) << 8) + ((buffer[4] & 0x0f) << 4) + (buffer[5] & 0x0f);
						zoomValue = ((zoomValue >> 15) & 1) ? zoomValue | ((-1 >> 16) << 16) : zoomValue & 0xffff;
						p->set_CameraInfo_zoom(zoomValue);
						::SetEvent(p->m_hHandle2);
					}
				}
			}
		}
		Sleep(10);
	}
	return NULL;
}


BOOL PanAndTiltCameraControl::getRun_Status()
{
	return m_thread_run_flag;
}

#define CLIENT_CAMERA_SPEED_PAN_MIN 0
#define CLIENT_CAMERA_SPEED_PAN_MAX 24
#define CLIENT_CAMERA_SPEED_TILT_MIN 0
#define CLIENT_CAMERA_SPEED_TILT_MAX 20
PanAndTiltCameraControl::PanAndTiltCameraControl()
{
	m_flag_start = FALSE;
	memset(&m_addr, 0, sizeof(m_addr));
	memset(&m_buffer, 0, sizeof(m_buffer));
	m_addr_len = sizeof(struct sockaddr_in);
	m_send_socket = INVALID_SOCKET;

	move_speed_pan = CLIENT_CAMERA_SPEED_PAN_MAX;
	move_speed_tilt = CLIENT_CAMERA_SPEED_TILT_MAX;

	m_thread_run_flag = TRUE;
	heart_tid = ::CreateThread(NULL, NULL, recv_thread, (void *)(this), NULL, NULL);
}

PanAndTiltCameraControl::~PanAndTiltCameraControl()
{
	m_thread_run_flag = FALSE;
	WaitForSingleObject(heart_tid, INFINITE);
	if (m_flag_start == TRUE)
	{
		stopControl();
	}
	OutputDebugString(_T("相机控制类结束"));
}

int PanAndTiltCameraControl::startControl(const char addr[], const int port)
{
	if (m_flag_start == FALSE)
	{
		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = inet_addr(addr);
		m_addr.sin_port = htons(port);
		m_send_socket = socket(AF_INET, SOCK_DGRAM, 0);
		int Time = 1000 * 2;
		setsockopt(m_send_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&Time, sizeof(int));
		m_hHandle1 = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hHandle2 = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (INVALID_SOCKET == m_send_socket)
		{
			printf("error!\n");
			return FALSE;
		}
		m_flag_start = TRUE;
	}
	return TRUE;
}


void PanAndTiltCameraControl::stopControl()
{
	::CloseHandle(m_hHandle1);
	::CloseHandle(m_hHandle2);
	closesocket(m_send_socket);
	m_flag_start = FALSE;
}

int PanAndTiltCameraControl::send_net_cmd(char *cmd, int len)
{
	return sendto(m_send_socket, cmd, len, 0, (struct sockaddr *)&m_addr, m_addr_len);
}

void PanAndTiltCameraControl::setMoveSpeed(int speed_pan, int speed_tilt)
{
	if (speed_pan < CLIENT_CAMERA_SPEED_PAN_MIN)
		speed_pan = CLIENT_CAMERA_SPEED_PAN_MIN;
	if (speed_pan > CLIENT_CAMERA_SPEED_PAN_MAX)
		speed_pan = CLIENT_CAMERA_SPEED_PAN_MAX;
	if (speed_tilt < CLIENT_CAMERA_SPEED_TILT_MIN)
		speed_tilt = CLIENT_CAMERA_SPEED_TILT_MIN;
	if (speed_tilt > CLIENT_CAMERA_SPEED_TILT_MAX)
		speed_tilt = CLIENT_CAMERA_SPEED_TILT_MAX;

	move_speed_pan = speed_pan;
	move_speed_tilt = speed_tilt;
}

#define CLIENT_CAMERA_POSITTOIN_PAN_MIN -0x0990
#define CLIENT_CAMERA_POSITTOIN_PAN_MAX 0x0990
#define CLIENT_CAMERA_POSITTOIN_TILT_MIN -0x01B0
#define CLIENT_CAMERA_POSITTOIN_TILT_MAX 0x0510
#define POSITTION_TO_4BYTE(x) ((x >> 12) & 0x000f), ((x >> 8) & 0x000f), ((x >> 4) & 0x000f), (x & 0x000f)
BOOL PanAndTiltCameraControl::move(int posittion_pan, int posittion_tilt, int flag)
{
	if (posittion_pan < CLIENT_CAMERA_POSITTOIN_PAN_MIN)
		posittion_pan = CLIENT_CAMERA_POSITTOIN_PAN_MIN;
	if (posittion_pan > CLIENT_CAMERA_POSITTOIN_PAN_MAX)
		posittion_pan = CLIENT_CAMERA_POSITTOIN_PAN_MAX;

	if (posittion_tilt < CLIENT_CAMERA_POSITTOIN_TILT_MIN)
		posittion_tilt = CLIENT_CAMERA_POSITTOIN_TILT_MIN;
	if (posittion_tilt > CLIENT_CAMERA_POSITTOIN_TILT_MAX)
		posittion_tilt = CLIENT_CAMERA_POSITTOIN_TILT_MAX;

	char instruct[] = { 0x81, 0x01, 0x06, 0x02, move_speed_pan, move_speed_tilt, POSITTION_TO_4BYTE(posittion_pan), POSITTION_TO_4BYTE(posittion_tilt), 0xFF };
	if (flag == TRUE)
	{
		instruct[3] = 0x03;//相对坐标
	}
	else
	{
		instruct[3] = 0x02;//绝对坐标
	}
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

#define CLIENT_CAMERA_ZOOM_MIN 0x0000
#define CLIENT_CAMERA_ZOOM_MAX 0x4000
#define ZOOMPOSITION_TO_4BYTE(x) ((x >> 12) & 0x000f), ((x >> 8) & 0x000f), ((x >> 4) & 0x000f), (x & 0x000f)
BOOL PanAndTiltCameraControl::setZoom(int zoomPosition)
{
	if (zoomPosition < CLIENT_CAMERA_ZOOM_MIN)
		zoomPosition = CLIENT_CAMERA_ZOOM_MIN;
	if (zoomPosition > CLIENT_CAMERA_ZOOM_MAX)
		zoomPosition = CLIENT_CAMERA_ZOOM_MAX;

	char instruct[] = { 0x81, 0x01, 0x04, 0x47, ZOOMPOSITION_TO_4BYTE(zoomPosition), 0xFF };
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL PanAndTiltCameraControl::home()
{
	//返回源点
	char instruct[] = { 0x81, 0x01, 0x06, 0x04, 0xFF };
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
	{	
		return FALSE;
	}
	else
	{	
		return TRUE;
	}
}

BOOL PanAndTiltCameraControl::keepInstruct(int type, void* otherData)
{
	BOOL flag = FALSE;
	switch (type & PANandTILT_CTRL_PTZ_MASK){
	case 0x1000:
		flag=keepMove(type);
		break;
	case 0x2000:
		flag=keepZoom(type);
		break;
	case 0x3000:
		//对焦操作
		flag = keepFocus(type);
		break;
	default:
		break;
	}
	return flag;
}

BOOL PanAndTiltCameraControl::preset(int type, int id)
{
	//预置位
	if (id<0 || id>254)
		return FALSE;

	char instruct[] = { 0x81, 0x01, 0x04, 0x3F, 0x00, id, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_GOTO_PRESET://运行到预置位
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_SET_PRESET://设置当前位置为预置位
		instruct[4] = 0x01;
		break;
	case PANandTILT_CTRL_PTZ_CLE_PRESET://删除当前预置位
		instruct[4] = 0x00;
		break;
	default:
		instruct[4] = 0x02;
		break;
	}
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL PanAndTiltCameraControl::keepFocus(int type)
{
	char instruct[] = { 0x81, 0x01, 0x04, 0x08, 0x00, 0xFF };
	switch (type)
	{
	case PANandTILT_CTRL_PTZ_FOCUSMANUAL:
		instruct[3] = 0x38;
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSAUTO:
		instruct[3] = 0x38;
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSIN:
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSOUT:
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_FOCUSSTOP:
		break;
	default:
		instruct[3] = 0x38;
		instruct[4] = 0x02;
		break;
	}
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL PanAndTiltCameraControl::keepMove(int type)
{
	char instruct[] = { 0x81, 0x01, 0x06, 0x01, move_speed_pan, move_speed_tilt, 0x00, 0x00, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_STOP:
		instruct[6] = 0x03;
		instruct[7] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_UP:
		instruct[6] = 0x03;
		instruct[7] = 0x01;
		break;
	case PANandTILT_CTRL_PTZ_DOWN:
		instruct[6] = 0x03;
		instruct[7] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_LEFT:
		instruct[6] = 0x01;
		instruct[7] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_RIGHT:
		instruct[6] = 0x02;
		instruct[7] = 0x03;
		break;
	default:
		instruct[6] = 0x03;
		instruct[7] = 0x03;
		break;
	}
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL PanAndTiltCameraControl::keepZoom(int type)
{
	char instruct[] = { 0x81, 0x01, 0x04, 0x07,0x00, 0xFF };
	switch (type){
	case PANandTILT_CTRL_PTZ_ZOOMIN:
		instruct[4] = 0x02;
		break;
	case PANandTILT_CTRL_PTZ_ZOOMOUT:
		instruct[4] = 0x03;
		break;
	case PANandTILT_CTRL_PTZ_ZOOMSTOP:
		instruct[4] = 0x00;
		break;
	default:
		instruct[4] = 0x03;
		break;
	}
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	else
		return TRUE;
}

BOOL PanAndTiltCameraControl::getPosit(int *posit_pan, int *posit_tilt, int waitMillisecond)
{
	char instruct[] = { 0x81, 0x09, 0x06, 0x12, 0xFF };
	::ResetEvent(m_hHandle1);
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	if (::WaitForSingleObject(m_hHandle1, waitMillisecond) == WAIT_OBJECT_0)
	{
		*posit_pan = m_posit_pan;
		*posit_tilt = m_posit_tilt;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL PanAndTiltCameraControl::getZoom(int *zoomValue, int waitMillisecond)
{
	char instruct[] = { 0x81, 0x09, 0x04, 0x47, 0xFF };
	::ResetEvent(m_hHandle2);
	if (send_net_cmd(instruct, sizeof(instruct)) != sizeof(instruct))
		return FALSE;
	if (::WaitForSingleObject(m_hHandle2, waitMillisecond) == WAIT_OBJECT_0)
	{
		*zoomValue = m_zoomValue;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int PanAndTiltCameraControl::recv_CameraInfo(char* buffer)
{
	int len = recvfrom(m_send_socket, m_buffer, sizeof(m_buffer), 0, (struct sockaddr*)&m_addr, &m_addr_len);
	if (len>0)
	{
		memcpy(buffer, m_buffer,len);
	}
	return len;
}

void PanAndTiltCameraControl::set_CameraInfo_panTilt(int posit_pan, int posit_tilt)
{
	m_posit_pan = posit_pan;
	m_posit_tilt = posit_tilt;
}

void PanAndTiltCameraControl::set_CameraInfo_zoom(int zoomValue)
{
	m_zoomValue = zoomValue;
}

BOOL PanAndTiltCameraControl::getStart_Status()
{
	return m_flag_start;
}
