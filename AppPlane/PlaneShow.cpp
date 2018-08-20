#include"stdafx.h"
#include"AppPlane.h"
#include"ExtWndShadow.h"
#include"PlaneShow.h"
#include"resource.h"

extern  CAppPlaneApp theApp;


IMPLEMENT_DYNAMIC(CPlaneShow, CWnd)

CPlaneShow::CPlaneShow()
{
	m_pAppWnd = NULL;
}

CPlaneShow::~CPlaneShow()
{

}

void CPlaneShow::SendAppCmd(size_t cmd)
{
	m_pAppWnd->OnReceiveCmd(cmd,0);
}

void CPlaneShow::SetWndSize(size_t width, size_t height)
{
	m_Width = width;
	m_Height = height;
}

void CPlaneShow::SetLogOut(void(*pFunc)(CString))
{
	m_pOutPut = pFunc;
}


BEGIN_MESSAGE_MAP(CPlaneShow, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_REGISTERED_MESSAGE(theApp.ID_MENU_HIDEHINT, OnHindHint)
	ON_REGISTERED_MESSAGE(theApp.ID_MENU_SETTASK, OnShowSettingTask)
	ON_REGISTERED_MESSAGE(theApp.ID_MENU_SHOWHINT, OnShowHint)
END_MESSAGE_MAP()




int CPlaneShow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CPlaneShow::OnDestroy()
{
	__super::OnDestroy();

}

void CPlaneShow::OnPaint()
{
	CPaintDC dc(this);
	if (m_pAppWnd == NULL)
	{
		m_pAppWnd = new CPlaneCtrlDlg();

		if (!m_pAppWnd->Create(IDD_PLANECTRLDLG, this))
		{
			m_pOutPut(_T("Error creating IDD_PLANECTRLDLG"));
		}
		m_pAppWnd->ShowWindow(SW_SHOW);

	}
}


void CPlaneShow::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}

BOOL CPlaneShow::PreTranslateMessage(MSG* pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}


LRESULT CPlaneShow::OnShowHint(WPARAM wparam, LPARAM lparam)
{
	Invalidate(TRUE);
	(*m_pOutPut)(L"显示提示信息");
	return 0;
}

LRESULT CPlaneShow::OnHindHint(WPARAM wapram, LPARAM lparam)
{
	Invalidate(TRUE);
	(*m_pOutPut)(L"隐藏提示信息");
	return 0;
}

LRESULT CPlaneShow::OnShowSettingTask(WPARAM wparam, LPARAM lparam)
{
	AfxMessageBox(L"Add Code");
	return 0;
}

extern "C" _declspec(dllexport) char* GetAppModualMsg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return "咬牙控制飞机";
}

extern "C" _declspec(dllexport) CPlaneShow* CreateAppClass()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CPlaneShow* pClass = new CPlaneShow();
	return pClass;
}

extern "C" _declspec(dllexport) BOOL CreateAppWindow(CWnd* parent, CWnd* pViewer)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());			//for MFC dll stander
	if (pViewer == NULL)
		return FALSE;
	BOOL b = pViewer->Create(NULL,
		NULL,
		WS_CHILD | WS_VISIBLE,
		CRect(0, 0, 1300, 1300), parent, 0xFFF2);					//ID 号
	pViewer->ShowWindow(SW_SHOW);
	return b;
}

//动态添加菜单，根据应用的需要进行添加，不是必须的操
extern"C" _declspec(dllexport) BOOL	InstallExtMenu(CWnd* pChild, HWND Parent, UINT* NextMenuID)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	BOOL bReturn = FALSE;
	if (!pChild)					//dll 中的wnd为空，无法添加
		return FALSE;
	if (!Parent)
		return FALSE;
	HMENU hMenu = ::GetMenu(Parent);		//获取父窗口的菜单
	if (!hMenu)
		return FALSE;
	CMenu ParentMenu;
	ParentMenu.Attach(hMenu);
	CMenu Menu;
	if (!Menu.CreateMenu())						//创建菜单
		return FALSE;
	theApp.m_CommandToMessage.InitHashTable(3);		//预定3项，可修改

	NextMenuID = new UINT[3];
	NextMenuID[0] = theApp.ID_MENU_SHOWHINT;
	NextMenuID[1] = theApp.ID_MENU_HIDEHINT;
	NextMenuID[2] = theApp.ID_MENU_SETTASK;

	Menu.AppendMenu(MF_STRING, (UINT_PTR)(*NextMenuID), L"显示提示符");
	theApp.m_CommandToMessage.SetAt(*(NextMenuID)++, theApp.ID_MENU_SHOWHINT);	//设置映射
	Menu.AppendMenu(MF_STRING, (UINT_PTR)(*NextMenuID), L"隐藏提示符");
	theApp.m_CommandToMessage.SetAt(*(NextMenuID)++, theApp.ID_MENU_HIDEHINT);
	Menu.AppendMenu(MF_STRING, (UINT_PTR)(*NextMenuID), L"设置任务");
	theApp.m_CommandToMessage.SetAt(*(NextMenuID)++, theApp.ID_MENU_SETTASK);

	HMENU hsubMenu = Menu.Detach();
	ParentMenu.InsertMenuW(ParentMenu.GetMenuItemCount(), MF_BYPOSITION | MF_STRING | MF_POPUP, (UINT_PTR)hsubMenu,
		_T("飞机应用设置"));

	ParentMenu.Detach();
	CWnd::FromHandle(Parent)->DrawMenuBar();		//强制重绘菜单
	return TRUE;
}

extern"C" _declspec(dllexport)BOOL CheckExtMenuItem(UINT id)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	if (id == theApp.ID_MENU_HIDEHINT ||
		id == theApp.ID_MENU_SETTASK ||
		id == theApp.ID_MENU_SHOWHINT)		//判断是否是菜单消息
		return TRUE;
	else
		return FALSE;
}