
// pictureMFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CpictureMFCApp: 
// �йش����ʵ�֣������ pictureMFC.cpp
//

class CpictureMFCApp : public CWinApp
{
public:
	CpictureMFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CpictureMFCApp theApp;