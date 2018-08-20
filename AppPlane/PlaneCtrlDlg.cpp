// PlaneCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AppPlane.h"
#include "PlaneCtrlDlg.h"
#include "afxdialogex.h"


// CPlaneCtrlDlg 对话框
const int INITPLANE = 1;
const int RUNINGPLANE = 2;
const unsigned char STABLE = 0x80;
const unsigned char SPEED = 0x0a;
IMPLEMENT_DYNAMIC(CPlaneCtrlDlg, CDialogEx)

CPlaneCtrlDlg::CPlaneCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlaneCtrlDlg::IDD, pParent)
{
	for (int i=0;i<16;i++)
	{
		m_nCmd[i] = 0x00;
	}
	m_nFlage = 1;
	m_nCount = 0;
	m_nStateMsg = _T("");
}

CPlaneCtrlDlg::~CPlaneCtrlDlg()
{
}

void CPlaneCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM1, m_ncomctrl);
	DDX_Control(pDX , IDC_SLEFTORRIGHT , m_nSLeftorRight);
	DDX_Control(pDX , IDC_SUPORBACK , m_nSUporBack);
	DDX_Control(pDX , IDC_SACCELERATE , m_nSAccelator);
	DDX_Control(pDX , IDC_LISTSTATE, m_nInfoList);
	DDX_Control(pDX , IDC_COMNUM , m_nPortnum);
	DDX_Text(pDX , IDC_ESENDCMD , m_CmdChar);
}


BEGIN_MESSAGE_MAP(CPlaneCtrlDlg, CDialogEx)
	ON_BN_CLICKED(ID_CONNECTORDISCONNECT, &CPlaneCtrlDlg::OnBnClickedConnectordisconnect)
	ON_BN_CLICKED(ID_START, &CPlaneCtrlDlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_STOP, &CPlaneCtrlDlg::OnBnClickedStop)
	ON_MESSAGE(MY_WM_CONTROLCMD , &CPlaneCtrlDlg::OnReceiveCmd)
	ON_MESSAGE(WM_HOTKEY , OnHotKey)
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BSENDTEST, &CPlaneCtrlDlg::OnBnClickedBsendtest)
END_MESSAGE_MAP()


// CPlaneCtrlDlg 消息处理程序


void CPlaneCtrlDlg::OnBnClickedConnectordisconnect()
{
	UpdateData(TRUE);
	int number = m_nPortnum.GetCurSel()+1;
	if(m_ncomctrl.get_PortOpen())
	{
		m_ncomctrl.put_PortOpen(FALSE);
	}
	m_ncomctrl.put_CommPort(number);
	m_ncomctrl.put_Settings(_T("9600,n,8,1"));
	m_ncomctrl.put_PortOpen(TRUE);
	if(m_ncomctrl.get_PortOpen())
	{
		GetDlgItem(ID_START)->EnableWindow(TRUE);
		GetDlgItem(ID_STOP)->EnableWindow(FALSE);
		m_nStateMsg = _T("连接成功");
		Showlog();
	}
}


void CPlaneCtrlDlg::OnBnClickedStart()
{
	if(m_nFlage==1)
	{
		m_nFlage++;
		InitPlane();
	}
	GetDlgItem(ID_START)->EnableWindow(FALSE);
	GetDlgItem(ID_STOP)->EnableWindow(TRUE);
	m_nStateMsg = _T("启动成功");
	Showlog();
}


void CPlaneCtrlDlg::OnBnClickedStop()
{
	m_nLeftorRight			= m_nCmd[8] = 0x80;
	m_nUporBack				= m_nCmd[9] = 0x80;
	m_nAccelator			= m_nCmd[11]= 0x80;
	m_nHover = 0xff;
	UpdateState();

	GetDlgItem(ID_START)->EnableWindow(TRUE);
	GetDlgItem(ID_STOP)->EnableWindow(FALSE);

	m_nStateMsg = _T("停止成功");
	Showlog();
}
/*
命令对应
左飞       2      左重
右飞       6      右重
前飞       3      左两次
后飞       7      右两次
油门拉升   4	  左长
油门下降   8      右长      
悬停       10 12   同时重 同时长
*/
LRESULT CPlaneCtrlDlg::OnReceiveCmd(WPARAM wParam,LPARAM lParam)
{
	m_nStateMsg = _T("");
	if(wParam==4)
	{
		m_nAccelator = 0xff;
		m_nHover = 0x00;
		m_nStateMsg = _T("油门拉升");
	}
	if(wParam==2)
	{
		m_nLeftorRight = 0x00;
		m_nHover = 0x00;
		m_nStateMsg = _T("左飞");
	}
	if(wParam==3)
	{
		m_nUporBack = 0x00;
		m_nHover = 0x00;
		m_nStateMsg = _T("前飞");
	}
	if(wParam==8)
	{
		m_nAccelator = 0x00;
		m_nHover = 0x00;
		m_nStateMsg = _T("油门下降");
	}
	if(wParam==6)
	{
		m_nLeftorRight = 0xff;
		m_nHover = 0x00;
		m_nStateMsg = _T("右飞");
	}
	if(wParam==7)
	{
		m_nUporBack = 0xff;
		m_nHover = 0x00;
		m_nStateMsg = _T("后飞");
	}
	if(wParam==10||wParam==12||wParam==0)
	{
		m_nHover = 0xff;
		m_nStateMsg = _T("悬停");
		m_nAccelator = 0x80;
		m_nUporBack = 0x80;
		m_nLeftorRight = 0x80;
	}

	UpdateState();
	Showlog();
	return 0;
}

void CPlaneCtrlDlg::SendCmd()
{
	CByteArray array;
	array.RemoveAll();
	array.SetSize(1);
	for (int i = 0; i<1; i++)													//仅第一字节有效
		array.SetAt(i, m_nCmd[i]);
	m_ncomctrl.put_Output(COleVariant(array));
}

//protect functions
void CPlaneCtrlDlg::ShowValue(DWORD id , unsigned char v)
{
	CEdit *pCtrl = (CEdit*)this->GetDlgItem(id);
	if(pCtrl==NULL)
		return;
	CString str;
	str.Format(_T("0x%02x") , v);
	pCtrl->SetWindowText(str);
}
 
void CPlaneCtrlDlg::UpdateState()
{
	CSliderCtrl* pctrl1 = (CSliderCtrl*)GetDlgItem(IDC_SLEFTORRIGHT);
	pctrl1->SetPos(m_nLeftorRight);
	CSliderCtrl* pctrl2 = (CSliderCtrl*)GetDlgItem(IDC_SUPORBACK);
	pctrl2->SetPos(m_nUporBack);
	CSliderCtrl* pctrl4 = (CSliderCtrl*)GetDlgItem(IDC_SACCELERATE);
	pctrl4->SetPos(m_nAccelator);
	this->SendMessage(WM_VSCROLL);
}

void CPlaneCtrlDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	CSliderCtrl* pctrl1 = (CSliderCtrl*)GetDlgItem(IDC_SLEFTORRIGHT);
	m_nLeftorRight = (unsigned char)pctrl1->GetPos();
	ShowValue(IDC_ELEFTORRIGHT , m_nLeftorRight);
	CSliderCtrl* pctrl2 = (CSliderCtrl*)GetDlgItem(IDC_SUPORBACK);
	m_nUporBack = (unsigned char)pctrl2->GetPos();
	ShowValue(IDC_EUPORBACK , m_nUporBack);
	CSliderCtrl* pctrl4 = (CSliderCtrl*)GetDlgItem(IDC_SACCELERATE);
	m_nAccelator = (unsigned char)pctrl4->GetPos();
	ShowValue(IDC_EACCELERATE , m_nAccelator);

	if(m_nHover==0xff)
	{
		m_nCmd[0] = 0x36;
	}
	else
	{
		if(m_nLeftorRight==0xff)
		{
			m_nCmd[0] = 0x33;
			m_nLeftorRight = 0x80;
		}
		else if(m_nLeftorRight==0x00)
		{
			m_nCmd[0] = 0x32;
			m_nLeftorRight = 0x80;
		}
		if(m_nUporBack==0xff)
		{
			m_nUporBack = 0x80;
			m_nCmd[0] = 0x35;
		}
		else if(m_nUporBack==0x00)
		{
			m_nUporBack = 0x80;
			m_nCmd[0] = 0x34;
		}
		if(m_nAccelator==0xff)
		{
			m_nAccelator = 0x80;
			m_nCmd[0] = 0x31;
		}
		else if(m_nAccelator==0x00)
		{
			m_nAccelator = 0x80;
			m_nCmd[0] = 0x30;
		}
	}
	SendCmd();
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}



unsigned char CPlaneCtrlDlg::sumdata(unsigned char data[] , int len)
{
	unsigned char sumda = 0;
	for (int i = 0;i<len;i++)
	{
		sumda += data[i];
	}
	return sumda;
}

BOOL CPlaneCtrlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(ID_START)->EnableWindow(FALSE);
	GetDlgItem(ID_STOP)->EnableWindow(FALSE);
	m_nLeftorRight = 0x80;
	m_nUporBack	   = 0x80;
	m_nAccelator   = 0xff;
	m_nSLeftorRight.SetRange(0 , 255);
	m_nSLeftorRight.SetPos(m_nLeftorRight);
	m_nSLeftorRight.SetTicFreq(1);
	m_nSUporBack.SetRange(0 , 255);
	m_nSUporBack.SetPos(m_nUporBack);
	m_nSUporBack.SetTicFreq(1);
	m_nSAccelator.SetRange(0x00 , 255);
	m_nSAccelator.SetPos(m_nAccelator);
	m_nSAccelator.SetTicFreq(1);
	ShowValue(IDC_ELEFTORRIGHT , m_nLeftorRight);
	ShowValue(IDC_EUPORBACK , m_nUporBack);
	ShowValue(IDC_EACCELERATE , m_nAccelator);

	CString str;
	for (int i=0;i<9;i++)
	{
		str.Format(_T("com%d"),i+1);
		m_nPortnum.AddString(str);
	}
	m_nPortnum.SetCurSel(5);
	
	::RegisterHotKey(m_hWnd ,ID_HOTKEY_STOP , NULL , VK_SPACE);

	return TRUE;  // return TRUE unless you set the focus to a control
	
}
HRESULT CPlaneCtrlDlg::OnHotKey(WPARAM wParam , LPARAM lParam)
{
	OnBnClickedStop();
	AfxMessageBox(_T("Plane Stop"));
	return 0;
}

void CPlaneCtrlDlg::InitPlane()
{
	m_nLeftorRight	= m_nCmd[0] = 0x80;
	m_nUporBack		= m_nCmd[9] = 0x80;
	m_nAccelator	= m_nCmd[11]= 0x00;
	m_nHover		=	0x00;
	UpdateState();
}

void CPlaneCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==RUNINGPLANE)
	{
		this->SendMessage(WM_VSCROLL);
	}
	if(nIDEvent==INITPLANE)
	{
		m_nCount += 255;
		if(m_nCount>=255)
		{
			KillTimer(INITPLANE);
			m_nAccelator = m_nCmd[11] = 0xff;
			m_nCount = 0;
		}
		else 
		{
			m_nCmd[11] += 10;
			m_nAccelator = m_nCmd[11];
		}
		UpdateState();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CPlaneCtrlDlg::Showlog()
{
	if(m_nInfoList.GetCount()>500)
	{
		m_nInfoList.DeleteString(m_nInfoList.GetCount()-1);
	}
	m_nInfoList.InsertString(0,m_nStateMsg);
}


void CPlaneCtrlDlg::OnBnClickedBsendtest()
{
	UpdateData(TRUE);
	WPARAM parm;
	if(m_CmdChar.Compare(L"0")==0)
	{
		parm = 4;
	}
	else if(m_CmdChar.Compare(L"1")==0)
	{
		parm = 0;
	}
	else if(m_CmdChar.Compare(L"2")==0)
	{
		parm = 1;
	}
	else if(m_CmdChar.Compare(L"3")==0)
	{
		parm = 5;
	}
	else if(m_CmdChar.Compare(L"4")==0)
	{
		parm = 2;
	}
	else if(m_CmdChar.Compare(L"5")==0)
	{
		parm = 6;
	}
	else if(m_CmdChar.Compare(L"6")==0)
	{
		parm = 8;
	}
	else
		parm = 100;
	this->SendMessage(MY_WM_CONTROLCMD , parm , 0);
}
