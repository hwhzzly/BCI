
// EEGShowerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EEGShower.h"
#include "EEGShowerDlg.h"
#include "afxdialogex.h"
#include "TeeChartDefines.h"
#include "surfaceseries.h"
#include "waterfallseries.h"
#include "SeriesTextField.h"
#include "aspect.h"
#include "Environment.h"
#include "teeopengl.h"
#include "Stringoper.h"
#include "trisurfaceseries.h"
#include "fastlineseries.h"
#include "Common.h"
#include "EEgMemManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString		DevicePath  = _T("../Debug/EmotivReader.dll");
UCHAR		Channels[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
int			CHannelnumber = 14;
size_t		INTERVAL = 8;		//the everage of 8 point
size_t		UPDATETIMER = 16;	//the receive data.
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
DWORD WINAPI WorkThread(LPVOID lparam);

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


// CEEGShowerDlg 对话框

CEEGShowerDlg::CEEGShowerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEEGShowerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bExit = false;
	m_HThread = NULL;
}

void CEEGShowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEECOMMANDER1, m_TeeCommander);
	DDX_Control(pDX, IDC_TCHART1, m_Tchart);
	DDX_Control(pDX , IDC_COMBOXMODE , m_Comboxmode);
}

BEGIN_MESSAGE_MAP(CEEGShowerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(EEGRECCMD , OnReceivCmd)
END_MESSAGE_MAP()


// CEEGShowerDlg 消息处理程序

BOOL CEEGShowerDlg::OnInitDialog()
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

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CRect rect;
	GetClientRect(&rect);
	AddEmotive();
	
	m_DataBuffer.ResizeManageData(CHannelnumber*(rect.Width()));
	m_DataBuffer.SetChannum(CHannelnumber);
	
	m_TeeCommander.SetChartLink(m_Tchart.GetChartLink());

	InitScreen(rect);


	m_Comboxmode.AddString(_T("瀑布模式"));
	m_Comboxmode.AddString(_T("Surface模式"));
	m_Comboxmode.AddString(_T("OpenGl模式"));
	m_Comboxmode.SetCurSel(0);
	m_length = 0;
	m_start = false;
// 	for (int i = 0;i<CHannelnumber;i++)
// 	{
// 		m_SeriesIndex[i].intVal = m_Tchart.AddSeries(scFastLine);
// 	}
	for (int i = 0;i<CHannelnumber;i++)
	{
		m_Series[i] = (CSeries)m_Tchart.Series(i);
	}
	m_Tchart.GetAspect().SetView3D(FALSE);

	CEEgMemManager* pManager = CEEgMemManager::Instance();
	pManager->SetProperry(SERVER);
	pManager->SetParent(this);
	pManager->Open();
	
	pManager->SendCmd(PRO_EEG_READY);		//be ready
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEEGShowerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEEGShowerDlg::OnPaint()
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
		CDialogEx::OnPaint();
		if(m_start)
			AddData();
	}

}

HCURSOR CEEGShowerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEEGShowerDlg::AddEmotive()
{
	m_Loader.SetDevicePath(DevicePath);
	if(!m_Loader.LoadDevice(&m_pDevice))
	{
		TRACE("Load Device Failded\n");
		m_pDevice = NULL;
		return;
	}
	m_pDevice->EB_SetSampleRate(128);
	m_pDevice->EB_SetChannel(Channels , CHannelnumber);
	m_pDevice->EB_SetBufferSize(8);
}

void CEEGShowerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//add code.
}

void CEEGShowerDlg::InitScreen(CRect rect)
{
	
}

void CEEGShowerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	EndThread();
	if(m_pDevice!=NULL)
	{
		m_pDevice->EB_Stop();
		m_pDevice->EB_CloseDevice();
	}

	m_Loader.Free();

	CEEgMemManager* pManger  = CEEgMemManager::Instance();
	pManger->Close();
	delete pManger;
}

void CEEGShowerDlg::UpdateData(double* pd , int size)
{
	m_DataBuffer.AppendData(pd , size);
	m_start = true;
	PostMessage(WM_PAINT);
}

void CEEGShowerDlg::ShowData(double* pd , int size)
{
	int index = m_Comboxmode.GetCurSel();
	switch(index)
	{
	case 0:
		AddData();
		break;
	case 1:
		Mode_Surface();
		break;
	case 2:
		Mode_OpenGl();
		break;
	default: 
		break;
	}
}
bool CEEGShowerDlg::StartThread(DWORD (WINAPI lp)(LPVOID))
{
	m_bExit = false;
	if(NULL==lp)
		return false;
	if(m_HThread==NULL)
	{
		m_HThread = CreateThread(NULL , 0, (LPTHREAD_START_ROUTINE)*lp , this , 0 , NULL);
		if(!m_HThread)
			return false;
		return true;
	}
	return true;
}

void CEEGShowerDlg::EndThread()
{
	m_bExit = true;
	if(m_HThread)
	{
		DWORD dwRet = WaitForSingleObject(m_HThread , 1000);
		if(dwRet==WAIT_OBJECT_0)
		{
			TRACE("Exit successed\n");
		}
		else
		{
			DWORD ret = 0;
			GetExitCodeProcess(m_HThread  , &ret);
			TerminateThread(m_HThread ,ret);
			TRACE("Exit the thread but may cause some problem\n");
		}
	}
	CloseHandle(m_HThread);
	m_HThread = NULL;
}
void CEEGShowerDlg::AddData()
{
	int numsample = m_DataBuffer.GetDataSize()/CHannelnumber;
	double meandata = 0;
	COleSafeArray xarray;
	COleSafeArray yarray;
	double tmpv(0);
	long	index(0);
	DWORD numElements[] = {numsample/INTERVAL};
	xarray.Create(VT_R8 , 1 , numElements);
	yarray.Create(VT_R8 , 1 , numElements);
	
// 	if(m_DataBuffer.GetDataSize()>=m_DataBuffer.GetBufsize())
// 	{
// 		//delete the data..
// 		int num = numsample/ INTERVAL;
// 		for (int i = 0;i<CHannelnumber;i++)
// 		{
// 			m_Series[i].DeleteRange(0 , num);
// 		}
// 	}
	for (int j = 0;j<CHannelnumber;j++)
	{
		for (int i = 0;i<numsample/(INTERVAL);i++)
		{
			double val = 0;
			for (int k = 0;k<INTERVAL;k++)
			{
				val += m_DataBuffer[i*CHannelnumber*INTERVAL+j+k*CHannelnumber];
			}
			val /= INTERVAL;
			index = i;
			tmpv = i;
			xarray.PutElement(&index , &tmpv);
			yarray.PutElement(&index , &val);
		}
		index = numsample/INTERVAL;
		m_Series[j].AddArray(index , yarray,xarray);
	}
	xarray.Destroy();
	yarray.Destroy();
}

void CEEGShowerDlg::SaveDate(CString Filename , ManagedData& data)
{
	ASSERT(Filename.GetLength()>0);
	//first clear the file..
	CFile file;
	if(!m_bSmapleFileOpen)
	{
		m_bSmapleFileOpen = file.Open(Filename , CFile::modeReadWrite);
	}
	if(!m_bSmapleFileOpen)
		return;
	else
	{
		file.SetLength(0);						//clear the data.
	}
//	CArchive ar(&file,CArchive::store|CArchive::);			//store the data
	Stringoper oper;
	string str;
	int numsample = data.GetDataSize()/data.GetChannum();
	for (int i = 0;i<numsample/(INTERVAL);i++)
	{
		for (int j = 0;j<data.GetChannum();j++)
		{
			double val = 0;
			str = "";
			for (int k = 0;k<INTERVAL;k++)
			{
				val += data[i*data.GetChannum()*INTERVAL+j+k*data.GetChannum()];
			}
			val /= INTERVAL;
//			ar<<i<<_T(";")<<j<<_T(";")<<val<<_T("\n");
			str += oper.doubletostring(val)+";"+oper.inttostring(i*100)+";"+oper.inttostring(j)+"\r\n";
			file.Write(str.c_str() , str.length());
		}
	}
	//close the file
//	ar.Close();
	file.Close();
	m_bSmapleFileOpen = false;
}

void CEEGShowerDlg::Mode_WaterFall()
{
	
	m_Tchart.GetAspect().GetOpenGL().SetActive(FALSE);
}

void CEEGShowerDlg::Mode_Surface()
{
// 	m_Tchart.RemoveAllSeries();
// 	m_SeriesIndex.vt=VT_INT;
// 	m_SeriesIndex.intVal= m_Tchart.AddSeries(scTriSurface);
	
}

void CEEGShowerDlg::Mode_OpenGl()
{
	
	m_Tchart.GetAspect().GetOpenGL().SetActive(TRUE);
	
}

double CEEGShowerDlg::MeanData(ManagedData& data)
{
	if(data.GetDataSize()==0)
		return 0;
	double sum = 0;
	for (int i = 0;i<data.GetDataSize();i++)
	{
		sum += data[i];
	}
	return sum/data.GetDataSize();
}

LRESULT CEEGShowerDlg::OnReceivCmd(WPARAM wparam , LPARAM lparam)
{
	CEEgMemManager* pManager = CEEgMemManager::Instance();
	string str;
	pManager->GetCmd(str);
	bool ret;
	if(IsEqual(str , PRO_EEG_START))
	{
		ret = StartShow();
		if(ret)
			pManager->SendCmd(PRO_OK);
		else
			pManager->SendCmd(PRO_FAIL);
	}
	if(IsEqual(str , PRO_EEG_PAUSE))
	{
		PauseShow();
		return 0;
	}
	if(IsEqual(str,PRO_EEG_RESTART))
	{
		ReStartShow();
		return 0;
	}
	if(IsEqual(str , PRO_EEG_STOP))
	{
		StopShow();
		return 0;
	}
	if(IsEqual(str , PRO_EEG_STORE))
	{
		StoreData(true);
	}
	if(IsEqual(str , PRO_EEG_UNSTORE))
	{
		StoreData(false);
	}
	if(IsEqual(str , PRO_EEG_EXIT))
	{
		this->PostMessageW(WM_CLOSE);
	}
	return 0;
}

bool	CEEGShowerDlg::StartShow()
{
	return StartThread(WorkThread);
}
void	CEEGShowerDlg::PauseShow()
{
	if(m_HThread==NULL)
		return;
	SuspendThread(m_HThread);
}

void CEEGShowerDlg::StopShow()
{
	if(m_pDevice!=NULL)
	{
		m_pDevice->EB_Stop();
		m_pDevice->EB_CloseDevice();
	}
	EndThread();
}

void CEEGShowerDlg::ReStartShow()
{
	if(m_HThread==NULL)
		return;
	ResumeThread(m_HThread);
}

void CEEGShowerDlg::StoreData(bool b)
{

}

DWORD WINAPI WorkThread(LPVOID lparam)
{
	OVERLAPPED ov;
	HANDLE m_GetDataEvent;
	CEEGShowerDlg* pDlg = (CEEGShowerDlg*)lparam;
	BasicDevice* pDevice = pDlg->m_pDevice;
	if(pDlg==NULL||pDevice==NULL)
		return 1;
	int buffersize = CHannelnumber*8;
	double *pdata = new double[buffersize];					
	if(!pDevice->EB_OpenDevice())
	{
		TRACE(_T("Open the Device Failed\n"));
		return 1;
	}
	if(!pDevice->EB_Startup())
	{
		TRACE(_T("start the device failed\n"));
		return 1;
	}
	int ret;
	m_GetDataEvent = CreateEvent(NULL,true,true,NULL);
	ov.hEvent = m_GetDataEvent;
	while(!pDlg->m_bExit)
	{
		ret = WaitForSingleObject(m_GetDataEvent,INFINITE);			//waiting for signal
		if(pDevice->EB_GetData(pdata,buffersize,&ov))
		{
			pDlg->UpdateData(pdata , buffersize);
		}
	}
	delete pdata;
	return 0;
}