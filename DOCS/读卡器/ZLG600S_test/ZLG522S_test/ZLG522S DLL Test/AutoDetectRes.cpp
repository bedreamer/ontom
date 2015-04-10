// AutoDetectRes.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "AutoDetectRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDetectRes dialog


CAutoDetectRes::CAutoDetectRes(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoDetectRes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoDetectRes)
	m_strATQ = _T("");
	m_strSAK = _T("");
	m_strUID = _T("");
	m_strBlock1 = _T("");
	m_strBlock2 = _T("");
	m_strBlock3 = _T("");
	m_strTX = _T("Œ¥ºÏ≤‚µΩø®");
	m_iCnt = 0;
	m_uiTimeOver = 500;
	//}}AFX_DATA_INIT
}


void CAutoDetectRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoDetectRes)
	DDX_Text(pDX, IDC_EDIT_ATQ, m_strATQ);
	DDX_Text(pDX, IDC_EDIT_SAK, m_strSAK);
	DDX_Text(pDX, IDC_EDIT_UID, m_strUID);
	DDX_Text(pDX, IDC_EDIT_BLOCK1, m_strBlock1);
	DDX_Text(pDX, IDC_EDIT_BLOCK2, m_strBlock2);
	DDX_Text(pDX, IDC_EDIT_BLOCK3, m_strBlock3);
	DDX_Text(pDX, IDC_EDIT_TX, m_strTX);
	DDX_Radio(pDX, IDC_RADIO_CNT, m_iCnt);
	DDX_Text(pDX, IDC_EDIT_TIME_OVER, m_uiTimeOver);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoDetectRes, CDialog)
	//{{AFX_MSG_MAP(CAutoDetectRes)
	ON_BN_CLICKED(IDC_BUTTON_READ_RES, OnButtonReadRes)
	ON_BN_CLICKED(IDC_BUTTON_GET_AUTO_RES, OnButtonGetAutoRes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoDetectRes message handlers

BOOL CAutoDetectRes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CAutoDetectRes::ShowStatus(LPCTSTR lpszText, unsigned int uiStatus)
{
	CString str;
	if (STATUS_SUCCESS == uiStatus)
	{
		str = _T("≥…π¶£°");
		str = lpszText + str;
		MessageBox(str, _T("≥…π¶"), MB_OK);
	}
	else
	{
		str.Format(_T(" ß∞‹£°\r\n¥ÌŒÛ¥˙¬Î£∫0x%04X"), uiStatus);
		str = lpszText + str;
		MessageBox(str, _T(" ß∞‹"), MB_OK);
	}
}

void CAutoDetectRes::OnButtonReadRes() 
{
	// TODO: Add your control notification handler code here
	AUTO_DETECT_RES AutoDetectRes;
	unsigned char ucTXMode;
	unsigned char Status;
	unsigned int i, nBytes;
	CString str;

	UpdateData(TRUE);
	m_strATQ = _T("");
	m_strSAK = _T("");
	m_strUID = _T("");
	m_strBlock1 = _T("");
	m_strBlock2 = _T("");
	m_strBlock3 = _T("");
	m_strTX = _T("ºÏ≤‚÷–......");
	UpdateData(FALSE);
	UpdateWindow();
	Status = ZLG522S_ReadAutoDetectRes(m_hCtrl, m_uiTimeOver, &ucTXMode, &AutoDetectRes);
	if (STATUS_SUCCESS != Status)
		m_strTX = _T("Œ¥ºÏ≤‚µΩø®");
	else
	{
		switch(ucTXMode)
		{
		case 1:
			m_strTX = _T("TX1…œºÏ≤‚µΩø®");
			break;

		case 2:
			m_strTX = _T("TX2…œºÏ≤‚µΩø®");
			break;

		case 3:
			m_strTX = _T("TX1°¢2ºÏ≤‚µΩø®");
			break;

		default:
			m_strTX = _T("Œ¥ºÏ≤‚µΩø®");
		}

		m_strATQ.Format(_T("%02X%02X"), AutoDetectRes.TAInfo.ATQ[1], AutoDetectRes.TAInfo.ATQ[0]);
		m_strSAK.Format(_T("%02X"), AutoDetectRes.TAInfo.SAK);

		for (i = 0; i < AutoDetectRes.TAInfo.UIDLen; i++)
		{
			str.Format(_T("%02X"), AutoDetectRes.TAInfo.UID[AutoDetectRes.TAInfo.UIDLen - i - 1]);
			m_strUID += str;
		}

		if (AutoDetectRes.nBytes)
		{// µ⁄1øÈ
			nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
			AutoDetectRes.nBytes -= nBytes;
			for (i = 0; i < nBytes; i++)
			{
				str.Format(_T("%02X"), AutoDetectRes.Buf[i]);
				m_strBlock1 += str;
			}
			if (AutoDetectRes.nBytes)
			{// µ⁄2øÈ
				nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
				AutoDetectRes.nBytes -= nBytes;
				for (i = 0; i < nBytes; i++)
				{
					str.Format(_T("%02X"), AutoDetectRes.Buf[i + 16]);
					m_strBlock2 += str;
				}
				if (AutoDetectRes.nBytes)
				{// µ⁄3øÈ
					nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
					AutoDetectRes.nBytes -= nBytes;
					for (i = 0; i < nBytes; i++)
					{
						str.Format(_T("%02X"), AutoDetectRes.Buf[i + 32]);
						m_strBlock1 += str;
					}
				}
			}		
		}
	}
	UpdateData(FALSE);
}

void CAutoDetectRes::OnButtonGetAutoRes() 
{
	// TODO: Add your control notification handler code here
	AUTO_DETECT_RES AutoDetectRes;
	unsigned char ucTXMode;
	unsigned char ucADMode;
	unsigned char Status;
	unsigned int i, nBytes;
	CString str;
	
	UpdateData(TRUE);
	m_strATQ = _T("");
	m_strSAK = _T("");
	m_strUID = _T("");
	m_strBlock1 = _T("");
	m_strBlock2 = _T("");
	m_strBlock3 = _T("");
	m_strTX = _T("Œ¥ºÏ≤‚µΩø®");
	ucADMode = (m_iCnt) ? 0x01 : 0x00;
	Status = ZLG522S_GetAutoDetectRes(m_hCtrl, ucADMode, &ucTXMode, &AutoDetectRes);
	if (STATUS_SUCCESS == Status)
	{
		switch(ucTXMode)
		{
		case 1:
			m_strTX = _T("TX1…œºÏ≤‚µΩø®");
			break;
			
		case 2:
			m_strTX = _T("TX2…œºÏ≤‚µΩø®");
			break;
			
		case 3:
			m_strTX = _T("TX1°¢2ºÏ≤‚µΩø®");
			break;
			
		default:
			m_strTX = _T("Œ¥ºÏ≤‚µΩø®");
		}
		
		m_strATQ.Format(_T("%02X%02X"), AutoDetectRes.TAInfo.ATQ[1], AutoDetectRes.TAInfo.ATQ[0]);
		m_strSAK.Format(_T("%02X"), AutoDetectRes.TAInfo.SAK);
		
		for (i = 0; i < AutoDetectRes.TAInfo.UIDLen; i++)
		{
			str.Format(_T("%02X"), AutoDetectRes.TAInfo.UID[AutoDetectRes.TAInfo.UIDLen - i - 1]);
			m_strUID += str;
		}
		
		if (AutoDetectRes.nBytes)
		{// µ⁄1øÈ
			nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
			AutoDetectRes.nBytes -= nBytes;
			for (i = 0; i < nBytes; i++)
			{
				str.Format(_T("%02X"), AutoDetectRes.Buf[i]);
				m_strBlock1 += str;
			}
			if (AutoDetectRes.nBytes)
			{// µ⁄2øÈ
				nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
				AutoDetectRes.nBytes -= nBytes;
				for (i = 0; i < nBytes; i++)
				{
					str.Format(_T("%02X"), AutoDetectRes.Buf[i + 16]);
					m_strBlock2 += str;
				}
				if (AutoDetectRes.nBytes)
				{// µ⁄3øÈ
					nBytes = (AutoDetectRes.nBytes > 16) ? 16 : AutoDetectRes.nBytes;
					AutoDetectRes.nBytes -= nBytes;
					for (i = 0; i < nBytes; i++)
					{
						str.Format(_T("%02X"), AutoDetectRes.Buf[i + 32]);
						m_strBlock1 += str;
					}
				}
			}		
		}
	}
	UpdateData(FALSE);
	ShowStatus(_T("ªÒ»°◊‘∂ØºÏ≤‚Ω·π˚"), Status);
}
