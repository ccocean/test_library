
// itcVideoCapture.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CitcVideoCaptureApp: 
// �йش����ʵ�֣������ itcVideoCapture.cpp
//

class CitcVideoCaptureApp : public CWinApp
{
public:
	CitcVideoCaptureApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CitcVideoCaptureApp theApp;