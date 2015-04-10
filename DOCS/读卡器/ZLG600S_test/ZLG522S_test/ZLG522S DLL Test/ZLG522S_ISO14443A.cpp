// ZLG522S_ISO14443A.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "ZLG522S_ISO14443A.h"
#include "ZLG522S.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_ISO14443A dialog

CZLG522S_ISO14443A::CZLG522S_ISO14443A(CWnd* pParent /*=NULL*/)
	: CDialog(CZLG522S_ISO14443A::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZLG522S_ISO14443A)
	m_strCardInfo = _T("");
	m_strATS = _T("");
	m_strRev = _T("");
	//}}AFX_DATA_INIT
}


void CZLG522S_ISO14443A::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZLG522S_ISO14443A)
	DDX_Control(pDX, IDC_EDIT_SEND, m_bsSend);
	DDX_Control(pDX, IDC_COMBO_REQ_CODE, m_ctrComReqCode);
	DDX_Text(pDX, IDC_EDIT_CARD_INFO, m_strCardInfo);
	DDV_MaxChars(pDX, m_strCardInfo, 100);
	DDX_Text(pDX, IDC_EDIT_ATS, m_strATS);
	DDV_MaxChars(pDX, m_strATS, 100);
	DDX_Text(pDX, IDC_EDIT_REV, m_strRev);
	DDV_MaxChars(pDX, m_strRev, 800);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZLG522S_ISO14443A, CDialog)
	//{{AFX_MSG_MAP(CZLG522S_ISO14443A)
	ON_BN_CLICKED(IDC_BUTTON_ACTI, OnButtonActi)
	ON_BN_CLICKED(IDC_BUTTON_RATS, OnButtonRats)
	ON_BN_CLICKED(IDC_BUTTON_DESELECT, OnButtonDeselect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_ISO14443A message handlers
BOOL CZLG522S_ISO14443A::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctrComReqCode.AddString(_T("空闲的（IDLE）"));
	m_ctrComReqCode.AddString(_T("所有的（ALL）"));
	m_ctrComReqCode.SetCurSel(1);

	m_bsSend.SetEditMode(EDIT_MODE_STR | EDIT_MODE_HEX);
	m_bsSend.SetEndian(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CZLG522S_ISO14443A::ShowStatus(LPCTSTR lpszText, unsigned int uiStatus)
{
	CString str;
	if (STATUS_SUCCESS == uiStatus)
	{
		str = _T("成功！");
		str = lpszText + str;
		MessageBox(str, _T("成功"), MB_OK);
	}
	else
	{
		str.Format(_T("失败！\r\n错误代码：0x%04X"), uiStatus);
		str = lpszText + str;
		MessageBox(str, _T("失败"), MB_OK);
	}
}


void CZLG522S_ISO14443A::OnButtonActi() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucReqCode;
	CString str;
	ACTIVEPARAA TypeAInfo;

	m_strCardInfo = _T("");
	m_strATS = _T("");
	ucReqCode = (m_ctrComReqCode.GetCurSel() == 0) ? ISO14443_3A_REQIDL : ISO14443_3A_REQALL;
	Status = ZLG522S_PiccActivateTypeA(m_hCtrl, 1, ucReqCode, &TypeAInfo);
	if (STATUS_SUCCESS == Status)
	{
		m_strCardInfo = _T("卡号（UID）：0x");
		for (int i = TypeAInfo.UIDLen; i != 0; i--)
		{
			str.Format(_T("%02X"), TypeAInfo.UID[i-1]);
			m_strCardInfo += str;
		}
		str.Format(_T("； ATQ：0x%02X%02X； SAK：0x%02X"), 
					TypeAInfo.ATQ[1], TypeAInfo.ATQ[0], TypeAInfo.SAK);
		m_strCardInfo += str;
	}
		
	UpdateData(FALSE);
	ShowStatus(_T("A型卡激活"), Status);
}


void CZLG522S_ISO14443A::OnButtonRats() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucaTempBuf[32];
	unsigned int nBytes;
	CString str;

	Status = ZLG522S_PiccRequestATS(m_hCtrl, 0x0A, ucaTempBuf, sizeof(ucaTempBuf), &nBytes);
	if (STATUS_SUCCESS == Status)
	{
		m_strATS= _T("ATS：0x");
		for (unsigned int i = 0; i < nBytes; i++)
		{
			str.Format(_T("%02X "), ucaTempBuf[i]);
			m_strATS += str;
		}
	}
	else
		m_strATS = _T("");
	UpdateData(FALSE);
	ShowStatus(_T("请求应答以选择（RATS）"), Status);
}

void CZLG522S_ISO14443A::OnButtonDeselect() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	
	Status = ZLG522S_PiccDeSelect(m_hCtrl);
	ShowStatus(_T("解除激活（DeSelect）"), Status);
	
}

void CZLG522S_ISO14443A::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucaTempBuf[CMD_MAX_SIZE];
	unsigned int nBytes;
	CString str, str1;

	nBytes = m_bsSend.GetData(ucaTempBuf, sizeof(ucaTempBuf));
	if (nBytes)
	{
		m_strRev = _T("");
		Status = ZLG522S_PiccTPCL(m_hCtrl, ucaTempBuf, nBytes, ucaTempBuf, sizeof(ucaTempBuf), &nBytes);
		if (STATUS_SUCCESS == Status)
		{
			for (unsigned int i = 0; i < nBytes; i++)
			{
				str.Format(_T("%02X"), ucaTempBuf[i]);
				m_strRev += str;
			}
		}
		UpdateData(FALSE);
		ShowStatus(_T("T=CL传输"), Status);
	}	
}
