// ZLG522S DLL TestDlg.h : header file
//

#if !defined(AFX_ZLG522SDLLTESTDLG_H__C944E445_F977_4DE5_BFEB_C125F707D172__INCLUDED_)
#define AFX_ZLG522SDLLTESTDLG_H__C944E445_F977_4DE5_BFEB_C125F707D172__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "HexEdit.h"

#include "SetModuleMode.h"
#include "ZLG522S_Mifare.h"
#include "ZLG522S_PlusCPU.h"
#include "ZLG522S_ISO14443A.h"

/////////////////////////////////////////////////////////////////////////////
// CZLG522SDLLTestDlg dialog

class CZLG522SDLLTestDlg : public CDialog
{
// Construction
public:
	CZLG522SDLLTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CZLG522SDLLTestDlg)
	enum { IDD = IDD_ZLG522SDLLTEST_DIALOG };
	CTabCtrl	m_tTab;
	CZLG522S_Mifare		m_tMifareTab;
	CZLG522S_ISO14443A	m_tISO14443ATab;
	CZLG522S_PlusCPU	m_tPlusCPU;
	CHexEdit	m_hexRegVal;
	CHexEdit	m_hexRegAdr;
	CComboBox	m_ctrComModuleType;
	CComboBox	m_ctrComComNo;
	CComboBox	m_ctrComBaudRate;
	CString	m_strDevInfo;
	int		m_iTxMode;
	BYTE	m_ucPauseTime;
	BYTE	m_ucWaitTime;

	HANDLE m_hCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZLG522SDLLTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CZLG522SDLLTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonGetDevInfo();
	afx_msg void OnButtonCfgPcd();
	afx_msg void OnButtonClosePcd();
	afx_msg void OnButtonReadReg();
	afx_msg void OnButtonWrateReg();
	afx_msg void OnButtonCardReset();
	afx_msg void OnButtonSetModuleWorkMode();
	afx_msg void OnRadioTxMode();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnSelchangeTabCardMode(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboModuleType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowStatus(LPCTSTR lpszText, unsigned int uiStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZLG522SDLLTESTDLG_H__C944E445_F977_4DE5_BFEB_C125F707D172__INCLUDED_)
