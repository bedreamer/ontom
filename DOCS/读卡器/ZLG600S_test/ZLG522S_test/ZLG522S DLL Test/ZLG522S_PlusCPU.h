#if !defined(AFX_ZLG522S_PLUSCPU_H__89AEA03F_B1B0_4280_9F89_13EE4EE187D0__INCLUDED_)
#define AFX_ZLG522S_PLUSCPU_H__89AEA03F_B1B0_4280_9F89_13EE4EE187D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZLG522S_PlusCPU.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_PlusCPU dialog

class CZLG522S_PlusCPU : public CDialog
{
// Construction
public:
	CZLG522S_PlusCPU(CWnd* pParent = NULL);   // standard constructor
	HANDLE	m_hCtrl;
// Dialog Data
	//{{AFX_DATA(CZLG522S_PlusCPU)
	enum { IDD = IDD_DIALOG_PLUSCPU };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZLG522S_PlusCPU)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZLG522S_PlusCPU)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZLG522S_PLUSCPU_H__89AEA03F_B1B0_4280_9F89_13EE4EE187D0__INCLUDED_)
