
// itcVideoCaptureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "itcVideoCapture.h"
#include "itcVideoCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg(); 

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CitcVideoCaptureDlg 对话框



CitcVideoCaptureDlg::CitcVideoCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CitcVideoCaptureDlg::IDD, pParent)
	, m_get_panPosit(0)
	, m_get_tiltPosit(0)
	, m_get_zoomValue(0)
	, m_fixedPoint_id(0)
	, m_valResult(0)
	, m_valCnt(0)
	, m_valPos(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uiHandle = 0;
	m_uiHandle2 = 0;
	m_bConnectFlag = FALSE;
	m_bStreamFlag = FALSE;
	m_process_flag = false;

	m_ptDst_flag = 0;//是否采集了标定点

	m_imgbufferYUV.create(HEIGHT_STUTRACK_IMG_ * 3 / 2, WIDTH_STUTRACK_IMG_, CV_8UC1);
}

void CitcVideoCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipAddr);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ipAddr2);
	DDX_Text(pDX, IDC_EDIT1, m_get_panPosit);
	DDX_Text(pDX, IDC_EDIT2, m_get_tiltPosit);
	DDX_Text(pDX, IDC_EDIT3, m_get_zoomValue);
	DDX_Text(pDX, IDC_EDIT1, m_get_panPosit);
	DDX_Text(pDX, IDC_EDIT2, m_get_tiltPosit);
	DDX_Text(pDX, IDC_EDIT3, m_get_zoomValue);
	DDX_Control(pDX, IDC_SLIDER1, m_slider_H);
	DDX_Control(pDX, IDC_SLIDER2, m_slider_V);
	DDX_Control(pDX, IDC_SLIDER3, m_slider_Z);
	DDX_Control(pDX, IDC_CHECK1, m_check_showPoint);
	DDX_Text(pDX, IDC_EDIT4, m_fixedPoint_id);
	DDV_MinMaxInt(pDX, m_fixedPoint_id, 0, 3);
	DDX_Text(pDX, IDC_TXTRESULT, m_valResult);
	DDX_Control(pDX, IDC_TXTCOUNT, m_valCount);
}

BEGIN_MESSAGE_MAP(CitcVideoCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CitcVideoCaptureDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON2, &CitcVideoCaptureDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CitcVideoCaptureDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CitcVideoCaptureDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CitcVideoCaptureDlg::OnBnClickedButton5)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON6, &CitcVideoCaptureDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CitcVideoCaptureDlg::OnBnClickedButton7)
	ON_MESSAGE(WM_USER_THREADEND, OnUserThreadend)
	ON_BN_CLICKED(IDC_RESET, &CitcVideoCaptureDlg::OnBnClickedReset)
END_MESSAGE_MAP()


// CitcVideoCaptureDlg 消息处理程序
#define CONVERT_0_1728_TO_1296_n432(x) (1296-x)
BOOL CitcVideoCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	BYTE nf1 = 192;
	BYTE nf2 = 168;
	BYTE nf3 = 17;
	BYTE nf4 = 54;
	m_ipAddr.SetAddress(nf1, nf2, nf3, nf4);
	m_ipAddr2.SetAddress(nf1, nf2, nf3, nf4+1);
	int iResult = 0;
	WSADATA wsaData = { 0 };
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		MessageBox(_T("WSAStartup failed!"), _T("Error"), MB_ICONERROR + MB_OK);
	}

	CWnd *pWnd = GetDlgItem(IDC_IMAGE2);
	CRect ShowRect;
	pWnd->SetWindowPos(NULL, 0, 0, WIDTH_STUTRACK_IMG_, HEIGHT_STUTRACK_IMG_, SWP_NOZORDER | SWP_NOMOVE);
	pWnd->GetClientRect(&ShowRect);
	m_pt[0].x = ShowRect.left + 10;
	m_pt[0].y = ShowRect.top + 10;
	m_pt[1].x = ShowRect.right - 10;
	m_pt[1].y = ShowRect.top + 10;
	m_pt[2].x = ShowRect.right - 10;
	m_pt[2].y = ShowRect.bottom - 10;
	m_pt[3].x = ShowRect.left + 10;
	m_pt[3].y = ShowRect.bottom - 10;

	m_ptDst[0].x = -100;
	m_ptDst[0].y = 14;
	m_ptDst[1].x = 616;
	m_ptDst[1].y = 14;
	m_ptDst[2].x = 616;
	m_ptDst[2].y = -374;
	m_ptDst[3].x = -100;
	m_ptDst[3].y = -374;

	pDC = GetDlgItem(IDC_IMAGE2)->GetDC();
	hdc = pDC->GetSafeHdc();

	penR.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	penB.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	tchData = (Tch_Data_t *)malloc(sizeof(Tch_Data_t));

	std::string str;
	std::ostringstream  ostr;
	ostr << (int)nf1 << "." << (int)nf2 << "." << (int)nf3 << "." << (int)(nf4);
	str = ostr.str();
	m_CameraControl.startControl(str.c_str(), 1259);//连接云台控制
	m_CameraControl.getPosit(&m_get_panPosit, &m_get_tiltPosit, 500);
	m_CameraControl.getZoom(&m_get_zoomValue, 500);
	//m_slider_V.SetRange(-432, 1296);
	m_slider_V.SetRange(0, 1728);
	m_slider_H.SetRange(-2448, 2448);
	m_slider_Z.SetRange(0, 0x4000);
	m_slider_V.SetPos(CONVERT_0_1728_TO_1296_n432(m_get_tiltPosit));
	m_slider_H.SetPos(m_get_panPosit);
	m_slider_Z.SetPos(m_get_zoomValue);
	UpdateData(FALSE);
	//m_Handle_imgOK = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CitcVideoCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void  FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

	BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

	memset(bmih, 0, sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = width;
	bmih->biHeight = origin ? abs(height) : -abs(height);
	bmih->biPlanes = 1;
	bmih->biBitCount = (unsigned short)bpp;
	bmih->biCompression = BI_RGB;

	if (bpp == 8)
	{
		RGBQUAD* palette = bmi->bmiColors;
		int i;
		for (i = 0; i < 256; i++)
		{
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}
void CitcVideoCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if (m_process_flag)
		{
			if (!m_imgbufferShow2.empty())
			{
				IplImage m_showImage(m_imgbufferShow2);
				uchar buffer[sizeof(BITMAPINFOHEADER)+1024];
				BITMAPINFO* bmi = (BITMAPINFO*)buffer;
				int bmp_w = m_showImage.width, bmp_h = m_showImage.height;
				int bpp = (m_showImage.depth & 255)*m_showImage.nChannels;
				//代替cvvimage的Bpp()函数
				FillBitmapInfo(bmi, bmp_w, bmp_h, bpp, m_showImage.origin);
				CRect ShowRect;
				GetDlgItem(IDC_IMAGE2)->GetClientRect(&ShowRect);
				CDC *dc = GetDlgItem(IDC_IMAGE2)->GetDC();
				SetStretchBltMode(dc->GetSafeHdc(), COLORONCOLOR);
				StretchDIBits(
					dc->GetSafeHdc(), ShowRect.left, ShowRect.top, ShowRect.Width(), ShowRect.Height(), 0, 0, bmp_w, bmp_h,
					m_showImage.imageData, bmi, DIB_RGB_COLORS, SRCCOPY);

				if (m_check_showPoint.GetCheck() == TRUE)
				{
					int r = 6;
					CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0)), *oldPen;
					CBrush Brush(RGB(0, 255, 0)), *oldbrush;
					oldbrush = dc->SelectObject(&Brush);
					oldPen = dc->SelectObject(&GreenPen);            //选择画笔

					dc->Ellipse(m_pt[0].x - r, m_pt[0].y - r, m_pt[0].x + r, m_pt[0].y + r);
					dc->Ellipse(m_pt[1].x - r, m_pt[1].y - r, m_pt[1].x + r, m_pt[1].y + r);
					dc->Ellipse(m_pt[2].x - r, m_pt[2].y - r, m_pt[2].x + r, m_pt[2].y + r);
					dc->Ellipse(m_pt[3].x - r, m_pt[3].y - r, m_pt[3].x + r, m_pt[3].y + r);

					dc->SelectObject(oldbrush);
					dc->SelectObject(oldPen);            //选择画笔
					GreenPen.DeleteObject();
					Brush.DeleteObject();
				}
				ReleaseDC(dc);
			}
		}
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CitcVideoCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CitcVideoCaptureDlg::OnBnClickedButton1()
{
	//连接1
	if (m_bConnectFlag == FALSE)
	{
		UpdateData();
		DWORD dIP;
		UINT m_uiPort = 5556; //80
		std::string m_strUname = "admin";
		std::string m_strPword = "admin";
		m_ipAddr.GetAddress(dIP);
		if (m_camera.login(&m_uiHandle, m_strUname, m_strPword, dIP, m_uiPort))
		{
			SetDlgItemText(IDC_BUTTON1,_T("断开"));
			MessageBox(_T("Login Success"), _T("msg"), MB_ICONINFORMATION);
			m_camera.StreamStart();
			SetTimer(IMAGE_REFRESH_TIMER1, 40, NULL);
			m_bConnectFlag = TRUE;
		}
		else
		{
			std::string Addr;
			std::ostringstream  ostr;
			ostr << ((dIP >> 24) & 255) << "." << ((dIP >> 16) & 255) << "." << ((dIP >> 8) & 255) << "." << (dIP & 255);
			Addr = ostr.str();
			std::string str = Addr + "\n" + "Connect Failure";
			MessageBox(CString(str.c_str()), _T("msg"), MB_ICONEXCLAMATION);
			m_uiHandle = 0;
			m_bConnectFlag = FALSE;
		}
		UpdateData(FALSE);
	}
	else
	{
		if (!m_camera.StreamStop())
		{
			MessageBox(_T("StopStream Failed1"), _T("msg"), MB_ICONEXCLAMATION);
		}
		KillTimer(IMAGE_REFRESH_TIMER1);
		if (m_camera.logout())
		{
			m_bConnectFlag = FALSE;
			SetDlgItemText(IDC_BUTTON1, _T("连接"));
		}
	}
}
//DWORD WINAPI  CitcVideoCaptureDlg::StreamDecoderThread(LPVOID pParam)
//{
//	CitcVideoCaptureDlg *pSamnetlibDlg = (CitcVideoCaptureDlg*)pParam;
//	unsigned char *dataBuffer = NULL;
//	while (pSamnetlibDlg->m_bStreamFlag==TRUE)
//	{
//		//OutputDebugString
//		(pSamnetlibDlg->m_lock_streamBuffer).Lock();
//		if ((pSamnetlibDlg->m_stream_buffer).size() > 0)
//		{
//			dataBuffer = (pSamnetlibDlg->m_stream_buffer).front();
//			(pSamnetlibDlg->m_stream_buffer).pop_front();
//		}
//		(pSamnetlibDlg->m_lock_streamBuffer).Unlock();
//		if (dataBuffer!=NULL)
//		{
//			pSamnetlibDlg->m_decoder.decoder(dataBuffer + sizeof(int), *((int*)dataBuffer), pSamnetlibDlg->m_height, pSamnetlibDlg->m_width, pSamnetlibDlg->m_imageBuffer.data);
//			free(dataBuffer);
//			dataBuffer = NULL;
//			::PostMessage(::FindWindow(NULL, L"itcVideoCapture"), WM_PAINT, NULL, NULL);
//			//::SendMessage(::FindWindow(NULL, L"itcVideoCapture"), WM_PAINT, NULL, NULL);//每一帧发送一次消息
//			//::SetEvent(pSamnetlibDlg->m_Handle_imgOK);
//			//cv::Mat temp = pSamnetlibDlg->m_imageBuffer;
//		}
//		else
//		{
//			Sleep(10);
//		}
//
//	}
//	return 0;
//}

void CitcVideoCaptureDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_process_flag = false;
	WaitForSingleObject(m_hThread_img_process, INFINITE);

	if (m_bConnectFlag == TRUE)
	{
		KillTimer(IMAGE_REFRESH_TIMER1);
		m_camera.StreamStop();
		m_camera.logout();
		m_bConnectFlag = FALSE;
	}
	if (m_bConnectFlag2 == TRUE)
	{
		KillTimer(IMAGE_REFRESH_TIMER2);
		m_camera2.StreamStop();
		m_camera2.logout();
		m_bConnectFlag2 = FALSE;
	}
	m_CameraControl.stopControl();
	WSACleanup();			//释放网络连接资源
	OutputDebugString(_T("退出！\n"));
	CDialogEx::OnClose();
}


void CitcVideoCaptureDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == IMAGE_REFRESH_TIMER1)
	{
		//PostMessage(WM_PAINT, NULL, NULL);
		//在界面上显示图像
		cv::Mat temp = m_camera.getImageBuffer();
		if (!temp.empty())
		{	
			cv::resize(temp, m_imgbufferYUV, cv::Size(m_imgbufferYUV.cols, m_imgbufferYUV.rows));
			cvtColor(m_imgbufferYUV, m_imgbufferShow, CV_YUV2BGR_I420);
			IplImage m_showImage(m_imgbufferShow);
			uchar buffer[sizeof(BITMAPINFOHEADER)+1024];
			BITMAPINFO* bmi = (BITMAPINFO*)buffer;
			int bmp_w = m_showImage.width, bmp_h = m_showImage.height;
			int bpp = (m_showImage.depth & 255)*m_showImage.nChannels;
			//代替cvvimage的Bpp()函数
			FillBitmapInfo(bmi, bmp_w, bmp_h, bpp, m_showImage.origin);
			CRect ShowRect;
			GetDlgItem(IDC_IMAGE1)->GetClientRect(&ShowRect);
			CDC *dc = GetDlgItem(IDC_IMAGE1)->GetDC();
			SetStretchBltMode(dc->GetSafeHdc(), COLORONCOLOR);
			StretchDIBits(
				dc->GetSafeHdc(), ShowRect.left, ShowRect.top, ShowRect.Width(), ShowRect.Height(), 0, 0, bmp_w, bmp_h,
				m_showImage.imageData, bmi, DIB_RGB_COLORS, SRCCOPY);
			if (m_check_showPoint.GetCheck() == TRUE)
			{
				int r = 6;
				int x = ShowRect.left + ShowRect.Width() / 2;
				int y = ShowRect.top + ShowRect.Height() / 2;
				CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0)), *oldPen;
				CBrush Brush(RGB(0, 255, 0)), *oldbrush;
				oldbrush = dc->SelectObject(&Brush);
				oldPen = dc->SelectObject(&GreenPen);            //选择画笔

				dc->Ellipse(x - r, y - r, x + r, y + r);

				dc->SelectObject(oldbrush);
				dc->SelectObject(oldPen);            //选择画笔
				GreenPen.DeleteObject();
				Brush.DeleteObject();
			}
			ReleaseDC(dc);
		}

	}

	if (nIDEvent == IMAGE_REFRESH_TIMER2)
	{
		if (!m_process_flag)
		{
			cv::Mat temp = m_camera2.getImageBuffer();
			if (!temp.empty())
			{
				cv::resize(temp, m_imgbufferYUV, cv::Size(m_imgbufferYUV.cols, m_imgbufferYUV.rows));
				cvtColor(m_imgbufferYUV, m_imgbufferShow2, CV_YUV2BGR_I420);
				IplImage m_showImage(m_imgbufferShow2);
				uchar buffer[sizeof(BITMAPINFOHEADER)+1024];
				BITMAPINFO* bmi = (BITMAPINFO*)buffer;
				int bmp_w = m_showImage.width, bmp_h = m_showImage.height;
				int bpp = (m_showImage.depth & 255)*m_showImage.nChannels;
				//代替cvvimage的Bpp()函数
				FillBitmapInfo(bmi, bmp_w, bmp_h, bpp, m_showImage.origin);
				CRect ShowRect;
				GetDlgItem(IDC_IMAGE2)->GetClientRect(&ShowRect);
				CDC *dc = GetDlgItem(IDC_IMAGE2)->GetDC();
				SetStretchBltMode(dc->GetSafeHdc(), COLORONCOLOR);
				StretchDIBits(
					dc->GetSafeHdc(), ShowRect.left, ShowRect.top, ShowRect.Width(), ShowRect.Height(), 0, 0, bmp_w, bmp_h,
					m_showImage.imageData, bmi, DIB_RGB_COLORS, SRCCOPY);
				CPoint a, b, c, d;
				pOldPen = pDC->SelectObject(&penR);
				for (int i = 0; i < 480; i += (480 / 10))
				{
					a.x = i; a.y = 0;
					b.x = i; b.y = 264;
					pDC->MoveTo(a);
					pDC->LineTo(b);
				}
				pOldPen = pDC->SelectObject(&penB);
				a.x = 96; a.y = 0;
				b.x = 96 + 48 * 5; b.y = 0;
				c.x = 96 + 48 * 5; c.y = 264;
				d.x = 96; d.y = 264;
				pDC->MoveTo(a);
				pDC->LineTo(b);
				pDC->MoveTo(b);
				pDC->LineTo(c);
				pDC->MoveTo(c);
				pDC->LineTo(d);
				pDC->MoveTo(d);
				pDC->LineTo(a);

				if (m_check_showPoint.GetCheck() == TRUE)
				{
					int r = 6;
					CPen GreenPen(PS_SOLID, 1, RGB(0, 255, 0)), *oldPen;
					CBrush Brush(RGB(0, 255, 0)), *oldbrush;
					oldbrush = dc->SelectObject(&Brush);
					oldPen = dc->SelectObject(&GreenPen);            //选择画笔

					dc->Ellipse(m_pt[0].x - r, m_pt[0].y - r, m_pt[0].x + r, m_pt[0].y + r);
					dc->Ellipse(m_pt[1].x - r, m_pt[1].y - r, m_pt[1].x + r, m_pt[1].y + r);
					dc->Ellipse(m_pt[2].x - r, m_pt[2].y - r, m_pt[2].x + r, m_pt[2].y + r);
					dc->Ellipse(m_pt[3].x - r, m_pt[3].y - r, m_pt[3].x + r, m_pt[3].y + r);

					dc->SelectObject(oldbrush);
					dc->SelectObject(oldPen);            //选择画笔
					GreenPen.DeleteObject();
					Brush.DeleteObject();
				}
				ReleaseDC(dc);
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CitcVideoCaptureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pS = (CSliderCtrl*)pScrollBar;
	if ((void*)pScrollBar == (void*)&m_slider_Z)
	{
		m_get_zoomValue = pS->GetPos();
		m_CameraControl.setZoom(m_get_zoomValue);
	}

	if ((void*)pScrollBar == (void*)&m_slider_H)
	{
		m_get_panPosit = pS->GetPos();
		m_CameraControl.move(m_get_panPosit, m_get_tiltPosit, FALSE);
	}
	UpdateData(FALSE);
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CitcVideoCaptureDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pS = (CSliderCtrl*)pScrollBar;
	if ((void*)pScrollBar == (void*)&m_slider_V)
	{
		m_get_tiltPosit = CONVERT_0_1728_TO_1296_n432(pS->GetPos());
		m_CameraControl.move(m_get_panPosit, m_get_tiltPosit, FALSE);
	}
	UpdateData(FALSE);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CitcVideoCaptureDlg::OnBnClickedButton2()
{
	//获取位置参数
	m_CameraControl.getPosit(&m_get_panPosit, &m_get_tiltPosit, 500);
	m_CameraControl.getZoom(&m_get_zoomValue, 500);
	UpdateData(FALSE);
}


void CitcVideoCaptureDlg::OnBnClickedButton3()
{
	//源点位置
	m_CameraControl.home();
}


void CitcVideoCaptureDlg::OnBnClickedButton4()
{
	//启动跟踪
	if (m_bConnectFlag2 == TRUE)
	{
		if (!m_process_flag)
		{
			m_process_flag = true;
			m_hThread_img_process = ::CreateThread(NULL, NULL, imageProcessThread, (void *)(this), NULL, NULL);
			SetDlgItemText(IDC_BUTTON4, _T("停止跟踪"));
		}
		else
		{
			m_process_flag = false;
			WaitForSingleObject(m_hThread_img_process, INFINITE);
			SetDlgItemText(IDC_BUTTON4, _T("启动跟踪"));
		}
	}
}

LRESULT CitcVideoCaptureDlg::OnUserThreadend(WPARAM wParam, LPARAM lParam)
{
	tempstr.Format(_T("%d"), m_valResult);
	GetDlgItem(IDC_TXTRESULT)->SetWindowText(tempstr);
	tempstr.Format(_T("%d"), m_valCnt);
	GetDlgItem(IDC_TXTCOUNT)->SetWindowText(tempstr);
	tempstr.Format(_T("%d"), m_valPos);
	GetDlgItem(IDC_NUMPOS1)->SetWindowText(tempstr);
	return 0;
}

int outputDebugFormat(const char *szFormat, ...)
{
	char buffer[512] = { 0 };
	va_list args;
	va_start(args, szFormat);
	vsprintf_s(buffer, szFormat, args);
	va_end(args);
	OutputDebugStringA(buffer);
	return 0;
}

DWORD WINAPI CitcVideoCaptureDlg::imageProcessThread(LPVOID pParam)
{
	CitcVideoCaptureDlg* pDlg = (CitcVideoCaptureDlg*)pParam;

	cv::FileStorage fs("M.xml", cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		return false;
	}
	fs["m_transM"] >> pDlg->m_transM;
	fs.release();

	cv::Mat imgData;
	imgData.create(HEIGHT_STUTRACK_IMG_ * 3 / 2, WIDTH_STUTRACK_IMG_, CV_8UC1);

	
	memset(pDlg->tchData, 0, sizeof(Tch_Data_t));
	pDlg->tchData->sysData.width = WIDTH_STUTRACK_IMG_;
	pDlg->tchData->sysData.height = HEIGHT_STUTRACK_IMG_;

	TeaITRACK_Params *arg = (TeaITRACK_Params *)malloc(sizeof(TeaITRACK_Params));
	memset(arg, 0, sizeof(TeaITRACK_Params));
	arg->isSetParams = 1;
	Tch_Size_t _frame = { 480, 264 };
	Tch_Rect_t tch = { 44, 90, 383, 141 };//{0,75,480,150}
	Tch_Rect_t blk = { 239, 50, 166, 42 }; //{0, 35, 640, 50}
	Tch_Threshold_t threshold = { 2000, 3000, 75 };//{2000, 12000, 130}
	arg->blk = blk;
	arg->tch = tch;
	arg->threshold = threshold;
	arg->numOfPos = 11;
	arg->numOfSlide = 5;
	arg->frame = _frame;
	pDlg->tchData->sysData.callbackmsg_func = outputDebugFormat;
	Tch_Result_t *res = (Tch_Result_t *)malloc(sizeof(Tch_Result_t));
	memset(res, 0, sizeof(Tch_Result_t));
	CString str;
	int err = tch_Init(arg, pDlg->tchData);
	int num,numTemp=0;
	int cntOutside = 0, cntMultiple = 0;
	double _stand = 0, _move = 0;
	CPoint a, b, c, d;
	pDlg->pOldPen = pDlg->pDC->SelectObject(&pDlg->penB);
	//StuITRACK_Params *inst = (StuITRACK_Params *)malloc(sizeof(StuITRACK_Params));
	//memset(inst, 0, sizeof(StuITRACK_Params));
	//inst->clientParams.flag_setting = TRUE;
	//inst->clientParams.width = WIDTH_STUTRACK_IMG_;
	//inst->clientParams.height = HEIGHT_STUTRACK_IMG_;
	//inst->clientParams.stuTrack_debugMsg_flag = 1;
	//inst->clientParams.stuTrack_Draw_flag = TRUE;
	//inst->clientParams.stuTrack_move_threshold = THRESHOLD_STUTRACK_MOVE_DEFALUT_PARAMS;					//判定是移动目标的偏离阈值（相对于自身宽度的比值）
	//inst->clientParams.stuTrack_standCount_threshold = THRESHOLD_STUTRACK_STANDCOUNT_DEFALUT_PARAMS;		//判定为起立的帧数阈值
	//inst->clientParams.stuTrack_sitdownCount_threshold = THRESHOLD_STUTRACK_SITDOWNCOUNT_DEFALUT_PARAMS;	//判定为坐下的帧数阈值
	//inst->clientParams.stuTrack_moveDelayed_threshold = THRESHOLD_STUTRACK_MOVEDELAYED_DEFALUT_PARAMS;
	//inst->clientParams.stuTrack_direct_range = RANGE_STUTRACK_STANDDIRECT_DEFALUT_PARAMS;
	//inst->clientParams.stuTrack_direct_standard[0] = 260;
	//inst->clientParams.stuTrack_direct_standard[1] = 280;
	//inst->clientParams.stuTrack_direct_standard[2] = 260;
	//inst->clientParams.stuTrack_direct_standard[3] = 280;
	//inst->clientParams.stuTrack_stuWidth_standard[0] = 36;
	//inst->clientParams.stuTrack_stuWidth_standard[1] = 36;
	//inst->clientParams.stuTrack_stuWidth_standard[2] = 50;
	//inst->clientParams.stuTrack_stuWidth_standard[3] = 50;
	//inst->clientParams.stuTrack_vertex[0].x = 0;
	//inst->clientParams.stuTrack_vertex[0].y = 123;
	//inst->clientParams.stuTrack_vertex[1].x = WIDTH_STUTRACK_IMG_ - 1;
	//inst->clientParams.stuTrack_vertex[1].y = 123;
	//inst->clientParams.stuTrack_vertex[2].x = 0;
	//inst->clientParams.stuTrack_vertex[2].y = 223;
	//inst->clientParams.stuTrack_vertex[3].x = WIDTH_STUTRACK_IMG_ - 1;
	//inst->clientParams.stuTrack_vertex[3].y = 223;
	//inst->systemParams.callbackmsg_func = printf;
	//inst->systemParams.nsrcHeight = HEIGHT_STUTRACK_IMG_;
	//inst->systemParams.nsrcWidth = WIDTH_STUTRACK_IMG_;
	//StuITRACK_InteriorParams* interior_params_p = (StuITRACK_InteriorParams*)malloc(sizeof(StuITRACK_InteriorParams));//分配全局变量內存
	//memset(interior_params_p, 0, sizeof(StuITRACK_InteriorParams));
	//StuITRACK_OutParams_t *return_params = (StuITRACK_OutParams_t*)malloc(sizeof(StuITRACK_OutParams_t));
	//memset(return_params, 0, sizeof(StuITRACK_OutParams_t));

	std::vector<cv::Point2f> inpt;
	std::vector<cv::Point2f> outpt;
	Analysis_Timer_t *ptrOutside,*ptrMultiple;
	tch_startStatistics(pDlg->tchData);
	//stuTrack_initializeTrack(inst, interior_params_p);
	//unsigned int _time = gettime();
	while (pDlg->m_process_flag)
	{
		cv::Mat imgBuffer = pDlg->m_camera2.getImageBuffer();
		if (!imgBuffer.empty())
		{
			cv::resize(imgBuffer, imgData, cv::Size(imgData.cols, imgData.rows));
			//stuTrack_process(inst, interior_params_p, return_params, (char*)imgData.data, (char*)(imgData.data + (inst->systemParams.nsrcHeight*inst->systemParams.nsrcWidth)));
			
			tch_track((uchar*)imgData.data, (uchar*)(imgData.data + (480 * 264)), arg, pDlg->tchData, res);
			/*str.Format(_T("posIndex:%d\r\n"), pDlg->tchData->g_posIndex);
			OutputDebugString(str);*/
			
			//打印站立和移动的时间
			if (pDlg->tchData->analysis->standTimer.deltatime != _stand ||
				pDlg->tchData->analysis->moveTimer.deltatime != _move ||
				track_statisticGetCount(&pDlg->tchData->analysis->outTimer) != cntOutside ||
				track_statisticGetCount(&pDlg->tchData->analysis->mlpTimer) != cntMultiple)
			{
				str.Format(_T("stand: {%lf秒} , move: {%lf秒}\r\n"), pDlg->tchData->analysis->standTimer.deltatime/1000, pDlg->tchData->analysis->moveTimer.deltatime/1000);
				OutputDebugString(str);
				/*str.Format(_T("outside: %d次, multiple: %d次\r\n"), track_timerGetCount(pDlg->tchData->analysis->outTimer), track_timerGetCount(pDlg->tchData->analysis->mlpTimer));
				OutputDebugString(str);*/
				_stand = pDlg->tchData->analysis->standTimer.deltatime;
				_move = pDlg->tchData->analysis->moveTimer.deltatime;
				cntOutside = track_statisticGetCount(&pDlg->tchData->analysis->outTimer);
				cntMultiple = track_statisticGetCount(&pDlg->tchData->analysis->mlpTimer);
				/*str.Format(_T("%d次下讲台持续时间："), cntOutside);
				OutputDebugString(str);
				for (int i = 0; i < cntOutside; i++)
				{
					if (i == cntOutside - 1)
					{
						str.Format(_T("%d秒\r\n"), cntOutside);
					}
				}*/
				if (cntOutside>0)
				{
					str.Format(_T("走下讲台一共%d次，总时间为： %lf秒\r\n"), cntOutside, pDlg->tchData->analysis->outTimer.timer.deltatime/1000);
					OutputDebugString(str);
				}
				if (cntMultiple>0)
				{
					str.Format(_T("多目标一共%d次，总时间为： %lf秒\r\n"), cntMultiple, pDlg->tchData->analysis->mlpTimer.timer.deltatime / 1000);
					OutputDebugString(str);
				}
			}
			
			//tch_finishAnalysis(pDlg->tchData);
			//打印走下讲台的时间
			/*num = pDlg->tchData->analysis->cntOutside;
			if (num==1)
			{
				numTemp = 0;
			}
			
			if (num>numTemp)
			{
				str.Format(_T("Count:%d\r\n"), num);
				OutputDebugString(str);
				ptr = pDlg->tchData->analysis->outTimer;
				for (int i = 0; i < num; i++)
				{
					str.Format(_T("Time:{ %lf }"), ptr->deltatime);
					OutputDebugString(str);
					ptr = ptr->next;
				}
				OutputDebugString(_T("\r\n"));
				numTemp = num;
			}*/
			/*a.x = pDlg->tchData->pos_slide.left * 48;  a.y = arg->tch.y;
			b.x = (pDlg->tchData->pos_slide.right + 1) * 48;   b.y = arg->tch.y;
			c.x = (pDlg->tchData->pos_slide.right + 1) * 48;   c.y = arg->tch.y + arg->tch.height;
			d.x = pDlg->tchData->pos_slide.left * 48;  d.y = arg->tch.y + arg->tch.height;

			pDlg->pDC->MoveTo(a);
			pDlg->pDC->LineTo(b);
			pDlg->pDC->MoveTo(b);
			pDlg->pDC->LineTo(c);
			pDlg->pDC->MoveTo(c);
			pDlg->pDC->LineTo(d);
			pDlg->pDC->MoveTo(d);
			pDlg->pDC->LineTo(a);*/
			
			cvtColor(imgData, pDlg->m_imgbufferShow2, CV_YUV2BGR_I420);

			//pDlg->m_CameraControl.preset(PANandTILT_CTRL_PTZ_GOTO_PRESET, res->pos);
			pDlg->m_valResult = res->status; 
			pDlg->m_valCnt = pDlg->tchData->lastRectNum;
			pDlg->m_valPos = pDlg->tchData->pos_slide.center;
			::PostMessage(pDlg->GetSafeHwnd(), WM_USER_THREADEND, 0, 0);
			/*if (return_params->count_trackObj_bigMove > 0)
			{
				cv::Point rec = cv::Point(return_params->move_position.x, return_params->move_position.y);
				circle(pDlg->m_imgbufferShow2, rec, (return_params->moveObj_size.width+return_params->moveObj_size.height)/4, CV_RGB(255, 255, 0));
				inpt.clear();
				inpt.push_back(cv::Point2f(return_params->move_position.x, return_params->move_position.y));

				if (!pDlg->m_transM.empty())
				{
					perspectiveTransform(inpt, outpt, pDlg->m_transM);
					CString str;
					int size = ITC_IMAX(return_params->moveObj_size.width, return_params->moveObj_size.height);
					int zoom = ITC_IMAX(-48 * size + 15000,0);
					str.Format(_T("坐标：%d,%d,大小:%d，缩放：%d\n"), (int)outpt[0].x, (int)outpt[0].y, size, zoom);
					OutputDebugString(str);
					if (gettime() - _time>600)
					{	
						pDlg->m_CameraControl.move((int)outpt[0].x, (int)outpt[0].y, FALSE);
						pDlg->m_CameraControl.setZoom(zoom);
						_time = gettime();
					}
				}
			}*/
			::PostMessage(::FindWindow(NULL, L"itcVideoCapture"), WM_PAINT, NULL, NULL);
		}
		else
		{
			WaitForSingleObject(NULL,30);
		}
	}
	pDlg->pDC->SelectObject(pDlg->pOldPen);
	tch_finishStatistics(pDlg->tchData);
	//tch_startStatistics(pDlg->tchData);
	//stuTrack_stopTrack(inst, interior_params_p);
	tch_trackDestroy(pDlg->tchData);
	return 0;
}

void CitcVideoCaptureDlg::OnBnClickedButton5()
{
	//连接2
	if (m_bConnectFlag2 == FALSE)
	{
		UpdateData();
		DWORD dIP;
		UINT m_uiPort = 5556; //80
		std::string m_strUname = "admin";
		std::string m_strPword = "admin";
		m_ipAddr2.GetAddress(dIP);
		if (m_camera2.login(&m_uiHandle, m_strUname, m_strPword, dIP, m_uiPort))
		{
			SetDlgItemText(IDC_BUTTON5, _T("断开"));
			MessageBox(_T("Login Success"), _T("msg"), MB_ICONINFORMATION);
			m_camera2.StreamStart();
			SetTimer(IMAGE_REFRESH_TIMER2, 40, NULL);
			m_bConnectFlag2 = TRUE;
		}
		else
		{
			std::string Addr;
			std::ostringstream  ostr;
			ostr << ((dIP >> 24) & 255) << "." << ((dIP >> 16) & 255) << "." << ((dIP >> 8) & 255) << "." << (dIP & 255);
			Addr = ostr.str();
			std::string str = Addr + "\n" + "Connect Failure";
			MessageBox(CString(str.c_str()), _T("msg"), MB_ICONEXCLAMATION);
			m_uiHandle2 = 0;
			m_bConnectFlag2 = FALSE;
		}
		UpdateData(FALSE);
	}
	else
	{
		if (!m_camera2.StreamStop())
		{
			MessageBox(_T("StopStream Failed2"), _T("msg"), MB_ICONEXCLAMATION);
		}
		KillTimer(IMAGE_REFRESH_TIMER2);
		if (m_camera2.logout())
		{
			m_bConnectFlag2 = FALSE;
			SetDlgItemText(IDC_BUTTON5, _T("连接"));
		}
		m_process_flag = false;
		SetDlgItemText(IDC_BUTTON4, _T("启动跟踪"));
	}
}

BOOL flag_ButtonDown = FALSE;
int roquet_Id = 0;
bool roquet(CPoint point1, CPoint point2, int dis)
{
	if (abs(point1.x - point2.x) > dis)
	{
		return false;
	}
	else if (abs(point1.y - point2.y) > dis)
	{
		return false;
	}
	return true;
}
void CitcVideoCaptureDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int i = 0;
	CRect ShowRect;
	GetDlgItem(IDC_IMAGE2)->GetWindowRect(&ShowRect);
	ScreenToClient(ShowRect);
	point.x -= ShowRect.left;
	point.y -= ShowRect.top;
	for (i = 0; i < 4;i++)
	{
		if (roquet(point,m_pt[i],6))
		{	
			roquet_Id = i;
			flag_ButtonDown = TRUE;
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CitcVideoCaptureDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	flag_ButtonDown = FALSE;
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CitcVideoCaptureDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect ShowRect;
	GetDlgItem(IDC_IMAGE2)->GetWindowRect(&ShowRect);
	ScreenToClient(ShowRect);
	if (point.x > ShowRect.left+6 && point.x < ShowRect.right-6
		&&point.y > ShowRect.top+6 && point.y < ShowRect.bottom-6)
	{
		if (flag_ButtonDown == TRUE)
		{
			point.x -= ShowRect.left;
			point.y -= ShowRect.top;
			m_pt[roquet_Id] = point;
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CitcVideoCaptureDlg::OnBnClickedButton6()
{
	//标定
	if (!m_process_flag)
	{
		cv::Point2f ptSrc[4];
		ptSrc[0].x = m_pt[0].x;
		ptSrc[0].y = m_pt[0].y;
		ptSrc[1].x = m_pt[1].x;
		ptSrc[1].y = m_pt[1].y;
		ptSrc[2].x = m_pt[2].x;
		ptSrc[2].y = m_pt[2].y;
		ptSrc[3].x = m_pt[3].x;
		ptSrc[3].y = m_pt[3].y;

		if (!(m_ptDst_flag & 0x000f == 0x000f))
		{
			//没有采集足够的点
			m_ptDst[0].x = -100;
			m_ptDst[0].y = 14;
			m_ptDst[1].x = 616;
			m_ptDst[1].y = 14;
			m_ptDst[2].x = 616;
			m_ptDst[2].y = -374;
			m_ptDst[3].x = -100;
			m_ptDst[3].y = -374;
		}
		m_transM = getPerspectiveTransform(ptSrc, m_ptDst);
		cv::FileStorage fs("M.xml", cv::FileStorage::WRITE);
		if (fs.isOpened())
		{
			fs << "m_transM" << m_transM;
			fs.release();
		}
	}
}


void CitcVideoCaptureDlg::OnBnClickedButton7()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_process_flag)
	{
		UpdateData(TRUE);
		m_CameraControl.getPosit(&m_get_panPosit, &m_get_tiltPosit, 500);
		m_ptDst[m_fixedPoint_id].x = m_get_panPosit;
		m_ptDst[m_fixedPoint_id].y = m_get_tiltPosit;
		m_ptDst_flag |= (1 << m_fixedPoint_id);
	}
}


void CitcVideoCaptureDlg::OnBnClickedReset()
{
	// TODO:  在此添加控件通知处理程序代码

}
