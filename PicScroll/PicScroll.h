
// PicScroll.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPicScrollApp: 
// �йش����ʵ�֣������ PicScroll.cpp
//

class CPicScrollApp : public CWinApp
{
public:
	CPicScrollApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPicScrollApp theApp;