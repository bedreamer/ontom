// ZLG522S_PlusCPU.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "ZLG522S_PlusCPU.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_PlusCPU dialog


CZLG522S_PlusCPU::CZLG522S_PlusCPU(CWnd* pParent /*=NULL*/)
	: CDialog(CZLG522S_PlusCPU::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZLG522S_PlusCPU)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CZLG522S_PlusCPU::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZLG522S_PlusCPU)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZLG522S_PlusCPU, CDialog)
	//{{AFX_MSG_MAP(CZLG522S_PlusCPU)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_PlusCPU message handlers
