#pragma once
#include "mscomm1.h"
#include"Resource.h"
#include<afxdialogex.h>

// CPlaneCtrlDlg 对话框

class CPlaneCtrlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPlaneCtrlDlg)

public:
	CPlaneCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlaneCtrlDlg();

// 对话框数据
	enum { IDD = IDD_PLANECTRLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void			ShowValue(DWORD id , unsigned char value);
	void			UpdateState();
	unsigned char	sumdata(unsigned char data[] , int len);
	void			SendCmd();									//send the control command.

	void			InitPlane();
	void			Showlog();
	DECLARE_MESSAGE_MAP()

private:
	unsigned char		m_nLeftorRight;
	unsigned char		m_nUporBack;
	unsigned char		m_nAccelator;
	unsigned char		m_nHover;
	CSliderCtrl			m_nSLeftorRight;
	CSliderCtrl			m_nSUporBack;
	CSliderCtrl			m_nSAccelator;
	CListBox			m_nInfoList;
	CComboBox			m_nPortnum;

	unsigned char		m_nCmd[16];

	int					m_nFlage;
	int 				m_nCount;
	CString				m_nStateMsg;
	CString				m_CmdChar;
public:
	CMscomm1 m_ncomctrl;
	afx_msg void OnBnClickedConnectordisconnect();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnReceiveCmd(WPARAM wParam,LPARAM lParam);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg HRESULT OnHotKey(WPARAM wParam , LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBsendtest();

};
