
// EEGShower.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEEGShowerApp:
// �йش����ʵ�֣������ EEGShower.cpp
//

class CEEGShowerApp : public CWinApp
{
public:
	CEEGShowerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEEGShowerApp theApp;