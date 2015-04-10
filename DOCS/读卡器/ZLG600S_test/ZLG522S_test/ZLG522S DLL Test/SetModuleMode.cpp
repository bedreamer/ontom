// SetModuleMode.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "SetModuleMode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetModuleMode dialog


CSetModuleMode::CSetModuleMode(CWnd* pParent /*=NULL*/)
	: CDialog(CSetModuleMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetModuleMode)
	m_bChangeAutoDetect = FALSE;
	m_bChangeBaudRate = FALSE;
	m_bChangeModuleAdr = FALSE;
	m_bChangeModuleMode = FALSE;
	//}}AFX_DATA_INIT
}


void CSetModuleMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetModuleMode)
	DDX_Check(pDX, IDC_CHECK_CHANGE_AUTO_DETECT, m_bChangeAutoDetect);
	DDX_Check(pDX, IDC_CHECK_CHANGE_BAUD_RATE, m_bChangeBaudRate);
	DDX_Check(pDX, IDC_CHECK_CHANGE_MODULE_ADR, m_bChangeModuleAdr);
	DDX_Check(pDX, IDC_CHECK_CHANGE_WORK_MODE, m_bChangeModuleMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetModuleMode, CDialog)
	//{{AFX_MSG_MAP(CSetModuleMode)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_WORK_MODE, OnCheckChangeWorkMode)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_BAUD_RATE, OnCheckChangeBaudRate)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_MODULE_ADR, OnCheckChangeModuleAdr)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_AUTO_DETECT, OnCheckChangeAutoDetect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetModuleMode message handlers

void CSetModuleMode::OnCheckChangeWorkMode() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GetDlgItem(IDC_COMBO_COMM_MODE)->EnableWindow(m_bChangeModuleMode);
	GetDlgItem(IDC_COMBO_WORK_MODE)->EnableWindow(m_bChangeModuleMode);
}

void CSetModuleMode::OnCheckChangeBaudRate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	GetDlgItem(IDC_COMBO_BAUD_RATE_NO)->EnableWindow(m_bChangeBaudRate);	

}

void CSetModuleMode::OnCheckChangeModuleAdr() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	GetDlgItem(IDC_EDIT_MODULE_ADR)->EnableWindow(m_bChangeModuleAdr);
}

void CSetModuleMode::OnCheckChangeAutoDetect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
//	GetDlgItem(IDC_COMBO_AUTO_DETECT)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_COMBO_TXMODE)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_COMBO_REQ_MODE)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_COMBO_AUTH_MODE)->EnableWindow(m_bChangeAutoDetect);

	GetDlgItem(IDC_COMBO_SECTOR)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_COMBO_BNR)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_COMBO_EXT)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_RADIO_CRYPTO1)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_RADIO_AES)->EnableWindow(m_bChangeAutoDetect);
	GetDlgItem(IDC_EDIT_KEY)->EnableWindow(m_bChangeAutoDetect);
}
