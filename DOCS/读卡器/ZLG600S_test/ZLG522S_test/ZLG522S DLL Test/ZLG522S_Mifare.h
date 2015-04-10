#if !defined(AFX_ZLG522S_MIFARE_H__1FBB4A1E_56F9_4AF3_AB99_B6850557EAE0__INCLUDED_)
#define AFX_ZLG522S_MIFARE_H__1FBB4A1E_56F9_4AF3_AB99_B6850557EAE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZLG522S_Mifare.h : header file
//
#include "HexEdit.h"
#include "BSEdit.h"
#include "ZLG522S.h"

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_Mifare dialog

class CZLG522S_Mifare : public CDialog
{
// Construction
public:
	CZLG522S_Mifare(CWnd* pParent = NULL);   // standard constructor
	HANDLE	m_hCtrl;
// Dialog Data
	//{{AFX_DATA(CZLG522S_Mifare)
	enum { IDD = IDD_DIALOG_MIFARE };
	CComboBox	m_ctrComAutoKeySector;
	CHexEdit	m_hexData;
	CComboBox	m_ctrComAuthKeySector;
	CComboBox	m_ctrComAuthSector;
	CHexEdit	m_hexAuthKey;
	CHexEdit	m_hexAutoKey;
	CBSEdit	m_bsUID;
	CComboBox	m_ctrComAuthMode;
	CComboBox	m_ctrComSBlock;
	CComboBox	m_ctrComDBlock;
	CComboBox	m_ctrComBlock;
	CHexEdit	m_hexKey;
	CComboBox	m_ctrComAutoTxMode;
	CComboBox	m_ctrComKeyType;
	CComboBox	m_ctrComAutoSector;
	CComboBox	m_ctrComAutoExt;
	CComboBox	m_ctrComAutoBNr;
	CComboBox	m_ctrComAutoAuthMode;
	CComboBox	m_ctrComAutoReqCode;
	CComboBox	m_ctrComReqCode;
	BOOL	m_bAutoInit;
	BOOL	m_bAutoSend;
	BOOL	m_bAutoCont;
	BOOL	m_bAutoHalt;
	CString	m_strATQ;
	CString	m_strSAK;
	CString	m_strUID;
	int		m_iAutoKeyType;
	long	m_lValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZLG522S_Mifare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZLG522S_Mifare)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonRequest();
	afx_msg void OnButtonAnti();
	afx_msg void OnButtonSelect();
	afx_msg void OnButtonActi();
	afx_msg void OnSelchangeComboAuthMode();
	afx_msg void OnButtonAuth();
	afx_msg void OnButtonRead();
	afx_msg void OnButtonWrite();
	afx_msg void OnButtonReadVal();
	afx_msg void OnButtonWriteVal();
	afx_msg void OnButtonIncVal();
	afx_msg void OnButtonDec();
	afx_msg void OnButtonRes();
	afx_msg void OnSelchangeComboAuthSector();
	afx_msg void OnSelchangeComboAutoSector();
	afx_msg void OnButtonAutoSetPara();
	afx_msg void OnSelchangeComboAutoAuthMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void ShowStatus(LPCTSTR lpszText, unsigned int uiStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZLG522S_MIFARE_H__1FBB4A1E_56F9_4AF3_AB99_B6850557EAE0__INCLUDED_)
