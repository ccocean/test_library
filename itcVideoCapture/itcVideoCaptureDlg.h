
// itcVideoCaptureDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "CMyLock.h"
#include "itcCamera.h"
#include "client_cameraControl.h"
#include "afxwin.h"
#include "tch_params.h"
#include "tch_track.h"
#include "Tch_Queue.h"

#define IMAGE_REFRESH_TIMER1 101
#define IMAGE_REFRESH_TIMER2 102
#define HEIGHT_STUTRACK_IMG_ 264
#define WIDTH_STUTRACK_IMG_ 480

#define WM_USER_THREADEND WM_USER + 1
// CitcVideoCaptureDlg 对话框
class CitcVideoCaptureDlg : public CDialogEx
{
// 构造
public:
	CitcVideoCaptureDlg(CWnd* pParent = NULL);	// 标准构造函数

	UINT m_uiHandle;
	UINT m_uiHandle2;
	BOOL m_bStreamFlag;
	BOOL m_bConnectFlag;
	BOOL m_bConnectFlag2;

	bool m_process_flag;
	HANDLE m_hThread_img_process;
	//HANDLE m_Handle_imgOK;
	camera m_camera;
	camera m_camera2;
	PanAndTiltCameraControl m_CameraControl;
	cv::Mat m_imgbufferYUV;
	cv::Mat m_imgbufferShow;
	cv::Mat m_imgbufferShow2;

	CDC *pDC;
	HDC hdc;

	CPen penR;
	CPen penB;
	CPen *pOldPen;//画笔

	CPoint m_pt[4];
	int m_ptDst_flag;
	cv::Point2f m_ptDst[4];
	cv::Mat m_transM;

	Tch_Data_t *tchData;
// 对话框数据
	enum { IDD = IDD_ITCVIDEOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	static DWORD WINAPI  imageProcessThread(LPVOID pParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CIPAddressCtrl m_ipAddr;
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CIPAddressCtrl m_ipAddr2;
	int m_get_panPosit;
	int m_get_tiltPosit;
	int m_get_zoomValue;
	CSliderCtrl m_slider_V;
	CSliderCtrl m_slider_H;
	CSliderCtrl m_slider_Z;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	CButton m_check_showPoint;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg LRESULT OnUserThreadend(WPARAM wParam, LPARAM lParam);
	int m_fixedPoint_id;
	CString tempstr;
	int m_valResult;
	afx_msg void OnBnClickedReset();
	CStatic m_valCount;
	int m_valCnt;
	int m_valPos;
};

