#if !defined(AFX_AUTODETECTRES_H__90FC5038_2397_426F_9F34_83B0983C4BC1__INCLUDED_)
#define AFX_AUTODETECTRES_H__90FC5038_2397_426F_9F34_83B0983C4BC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoDetectRes.h : header file
//
#include "ZLG522S.h"
/////////////////////////////////////////////////////////////////////////////
// CAutoDetectRes dialog

class CAutoDetectRes : public CDialog
{
// Construction
public:
	CAutoDetectRes(CWnd* pParent = NULL);   // standard constructor
	HANDLE	m_hCtrl;
	AUTO_DETECT_PAR AutoDetectPara;
// Dialog Data
	//{{AFX_DATA(CAutoDetectRes)
	enum { IDD = IDD_DIALOG_AUTO_RES };
	CString	m_strATQ;
	CString	m_strSAK;
	CString	m_strUID;
	CString	m_strBlock1;
	CString	m_strBlock2;
	CString	m_strBlock3;
	CString	m_strTX;
	int		m_iCnt;
	UINT	m_uiTimeOver;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDetectRes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoDetectRes)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonReadRes();
	afx_msg void OnButtonGetAutoRes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ShowStatus(LPCTSTR lpszText, unsigned int uiStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTODETECTRES_H__90FC5038_2397_426F_9F34_83B0983C4BC1__INCLUDED_)
