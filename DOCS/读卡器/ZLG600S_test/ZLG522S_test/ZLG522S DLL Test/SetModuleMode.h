#if !defined(AFX_SETMODULEMODE_H__E5EF64F5_0020_48E5_B65F_2D6181C30D1A__INCLUDED_)
#define AFX_SETMODULEMODE_H__E5EF64F5_0020_48E5_B65F_2D6181C30D1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetModuleMode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetModuleMode dialog

class CSetModuleMode : public CDialog
{
// Construction
public:
	CSetModuleMode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetModuleMode)
	enum { IDD = IDD_DIALOG_SET_WORK_MODE };
	BOOL	m_bChangeAutoDetect;
	BOOL	m_bChangeBaudRate;
	BOOL	m_bChangeModuleAdr;
	BOOL	m_bChangeModuleMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetModuleMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetModuleMode)
	afx_msg void OnCheckChangeWorkMode();
	afx_msg void OnCheckChangeBaudRate();
	afx_msg void OnCheckChangeModuleAdr();
	afx_msg void OnCheckChangeAutoDetect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETMODULEMODE_H__E5EF64F5_0020_48E5_B65F_2D6181C30D1A__INCLUDED_)
