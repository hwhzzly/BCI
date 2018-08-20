
// EEGShowerDlg.h : ͷ�ļ�
//

#pragma once
#include "BasicDevice.h"
#include "DeviceLoader.cpp"
#include "ManagedData.h"
#include "teecommander.h"
#include "tchart.h"
#include "seriestextsource.h"
#include "series.h"

// CEEGShowerDlg �Ի���

class CEEGShowerDlg : public CDialogEx
{
// ����
public:
	CEEGShowerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_EEGSHOWER_DIALOG };
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	void	UpdateData(double* pd , int size);

	bool	StartThread(DWORD (WINAPI lp)(LPVOID));
	void	EndThread();

	BasicDevice*					m_pDevice;
	bool							m_bExit;
private:

	DeviceLoader<BasicDevice>		m_Loader;
	ManagedData						m_DataBuffer;
	HANDLE							m_HThread;

	CSeries							m_Series[14];
	VARIANT							m_SeriesIndex[14];
	CTeeCommander					m_TeeCommander;
	CTChart							m_Tchart;
	CComboBox						m_Comboxmode;
	size_t							m_NewDataLength;

	size_t							m_length;
	bool							m_start;
	bool							m_bSmapleFileOpen;
private:   //function
	void		AddEmotive();
	void		InitScreen(CRect rect);

	void		ShowData(double* pd , int size);
	void		AddData();
	void		SaveDate(CString Filename , ManagedData& data);
	void		Mode_WaterFall();
	void		Mode_Surface();
	void		Mode_OpenGl();

	double		MeanData(ManagedData& data);

	bool		StartShow();
	void		StopShow();
	void		PauseShow();
	void		ReStartShow();
	void		StoreData(bool	b);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnReceivCmd(WPARAM wparam , LPARAM lparam);
	
};
