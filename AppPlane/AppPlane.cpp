// AppPlane.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "AppPlane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CAppPlaneApp

BEGIN_MESSAGE_MAP(CAppPlaneApp, CWinApp)
END_MESSAGE_MAP()


// CAppPlaneApp ����

CAppPlaneApp::CAppPlaneApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAppPlaneApp ����

CAppPlaneApp theApp;


// CAppPlaneApp ��ʼ��
UINT CAppPlaneApp::ID_MENU_HIDEHINT = 0x10001;
UINT CAppPlaneApp::ID_MENU_SHOWHINT = 0x10002;
UINT CAppPlaneApp::ID_MENU_SETTASK =  0x10003;
BOOL CAppPlaneApp::InitInstance()
{
	::OleInitialize(NULL);
	::AfxEnableControlContainer();
	CWinApp::InitInstance();

	return TRUE;
}
