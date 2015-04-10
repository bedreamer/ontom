#if !defined(AFX_ZLG522S_ISO14443A_H__927061F6_5AAF_4A6C_8B40_5CE343D45F3A__INCLUDED_)
#define AFX_ZLG522S_ISO14443A_H__927061F6_5AAF_4A6C_8B40_5CE343D45F3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZLG522S_ISO14443A.h : header file
//
#include "BSEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_ISO14443A dialog

class CZLG522S_ISO14443A : public CDialog
{
// Construction
public:
	CZLG522S_ISO14443A(CWnd* pParent = NULL);   // standard constructor
	HANDLE	m_hCtrl;
// Dialog Data
	//{{AFX_DATA(CZLG522S_ISO14443A)
	enum { IDD = IDD_DIALOG_ISO14443 };
	CBSEdit	m_bsSend;
	CComboBox	m_ctrComReqCode;
	CString	m_strCardInfo;
	CString	m_strATS;
	CString	m_strRev;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZLG522S_ISO14443A)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZLG522S_ISO14443A)
	afx_msg void OnButtonActi();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonRats();
	afx_msg void OnButtonDeselect();
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ShowStatus(LPCTSTR lpszText, unsigned int uiStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZLG522S_ISO14443A_H__927061F6_5AAF_4A6C_8B40_5CE343D45F3A__INCLUDED_)
