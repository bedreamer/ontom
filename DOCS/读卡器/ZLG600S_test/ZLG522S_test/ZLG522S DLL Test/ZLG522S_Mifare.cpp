// ZLG522S_Mifare.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "ZLG522S_Mifare.h"
#include "AutoDetectRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_Mifare dialog


CZLG522S_Mifare::CZLG522S_Mifare(CWnd* pParent /*=NULL*/)
	: CDialog(CZLG522S_Mifare::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZLG522S_Mifare)
	m_bAutoInit = TRUE;
	m_bAutoSend = TRUE;
	m_bAutoCont = TRUE;
	m_bAutoHalt = TRUE;
	m_strATQ = _T("");
	m_strSAK = _T("");
	m_strUID = _T("");
	m_iAutoKeyType = 0;
	m_lValue = 0;
	//}}AFX_DATA_INIT
}


void CZLG522S_Mifare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZLG522S_Mifare)
	DDX_Control(pDX, IDC_COMBO_AUTO_KEY_SECTOR, m_ctrComAutoKeySector);
	DDX_Control(pDX, IDC_EDIT_DATA, m_hexData);
	DDX_Control(pDX, IDC_COMBO_AUTH_KEY_SECTOR, m_ctrComAuthKeySector);
	DDX_Control(pDX, IDC_COMBO_AUTH_SECTOR, m_ctrComAuthSector);
	DDX_Control(pDX, IDC_EDIT_AUTH_KEY, m_hexAuthKey);
	DDX_Control(pDX, IDC_EDIT_AUTO_KEY, m_hexAutoKey);
	DDX_Control(pDX, IDC_EDIT_UID, m_bsUID);
	DDX_Control(pDX, IDC_COMBO_AUTH_MODE, m_ctrComAuthMode);
	DDX_Control(pDX, IDC_COMBO_SBLOCK, m_ctrComSBlock);
	DDX_Control(pDX, IDC_COMBO_DBLOCK, m_ctrComDBlock);
	DDX_Control(pDX, IDC_COMBO_BLOCK, m_ctrComBlock);
	DDX_Control(pDX, IDC_COMBO_AUTO_TXMODE, m_ctrComAutoTxMode);
	DDX_Control(pDX, IDC_COMBO_AUTO_SECTOR, m_ctrComAutoSector);
	DDX_Control(pDX, IDC_COMBO_AUTO_EXT, m_ctrComAutoExt);
	DDX_Control(pDX, IDC_COMBO_AUTO_BNR, m_ctrComAutoBNr);
	DDX_Control(pDX, IDC_COMBO_AUTO_AUTH_MODE, m_ctrComAutoAuthMode);
	DDX_Control(pDX, IDC_COMBO_AUTO_REQ_CODE, m_ctrComAutoReqCode);
	DDX_Control(pDX, IDC_COMBO_REQ_CODE, m_ctrComReqCode);
	DDX_Check(pDX, IDC_CHECK_AUTO_INIT, m_bAutoInit);
	DDX_Check(pDX, IDC_CHECK_AUTO_SEND, m_bAutoSend);
	DDX_Check(pDX, IDC_CHECK_AUTO_CONT, m_bAutoCont);
	DDX_Check(pDX, IDC_CHECK_AUTO_HALT, m_bAutoHalt);
	DDX_Text(pDX, IDC_EDIT_ATQ, m_strATQ);
	DDX_Text(pDX, IDC_EDIT_SAK, m_strSAK);
	DDX_Text(pDX, IDC_EDIT_UID, m_strUID);
	DDX_Radio(pDX, IDC_RADIO_AUTO_CRYPTO1, m_iAutoKeyType);
	DDX_Text(pDX, IDC_EDIT_VAL, m_lValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZLG522S_Mifare, CDialog)
	//{{AFX_MSG_MAP(CZLG522S_Mifare)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST, OnButtonRequest)
	ON_BN_CLICKED(IDC_BUTTON_ANTI, OnButtonAnti)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_ACTI, OnButtonActi)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTH_MODE, OnSelchangeComboAuthMode)
	ON_BN_CLICKED(IDC_BUTTON_AUTH, OnButtonAuth)
	ON_BN_CLICKED(IDC_BUTTON_READ, OnButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_READ_VAL, OnButtonReadVal)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_VAL, OnButtonWriteVal)
	ON_BN_CLICKED(IDC_BUTTON_INC_VAL, OnButtonIncVal)
	ON_BN_CLICKED(IDC_BUTTON_DEC_, OnButtonDec)
	ON_BN_CLICKED(IDC_BUTTON_RES, OnButtonRes)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTH_SECTOR, OnSelchangeComboAuthSector)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_SECTOR, OnSelchangeComboAutoSector)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SET_PARA, OnButtonAutoSetPara)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_AUTH_MODE, OnSelchangeComboAutoAuthMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZLG522S_Mifare message handlers
BOOL CZLG522S_Mifare::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	unsigned char ucaKey[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	unsigned int i, j;
	CString str;

	m_ctrComReqCode.AddString(_T("空闲的（IDLE）"));
	m_ctrComReqCode.AddString(_T("所有的（ALL）"));
	m_ctrComReqCode.SetCurSel(0);

	m_ctrComAutoTxMode.AddString(_T("TX1、TX2交替驱动"));
	m_ctrComAutoTxMode.AddString(_T("仅TX1交替驱动"));
	m_ctrComAutoTxMode.AddString(_T("仅TX2交替驱动"));
	m_ctrComAutoTxMode.AddString(_T("TX1、TX2同时驱动"));
	m_ctrComAutoTxMode.SetCurSel(3);

	m_ctrComAutoReqCode.AddString(_T("空闲的（IDLE）"));
	m_ctrComAutoReqCode.AddString(_T("所有的（ALL）"));
	m_ctrComAutoReqCode.SetCurSel(0);

	m_ctrComAutoAuthMode.AddString(_T("外部密钥A认证"));
	m_ctrComAutoAuthMode.AddString(_T("外部密钥B认证"));
	m_ctrComAutoAuthMode.AddString(_T("E2密钥A认证"));
	m_ctrComAutoAuthMode.AddString(_T("E2密钥B认证"));
	m_ctrComAutoAuthMode.AddString(_T("不认证"));
	m_ctrComAutoAuthMode.SetCurSel(0);
	GetDlgItem(IDC_COMBO_AUTO_KEY_SECTOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_AUTO_EXT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_AUTO_AES)->EnableWindow(FALSE);
	m_iAutoKeyType = 0;
	m_hexAutoKey.SetEndian(TRUE);
	m_hexAutoKey.SetMaxDataLen(6);
	m_hexAutoKey.SetData(ucaKey, 6);

	for (i = 0; i < 4; i++)
	{
		str.Format(_T("密钥扇区%d"), i);
		m_ctrComAutoKeySector.AddString(str);
		m_ctrComAuthKeySector.AddString(str);
	}
	m_ctrComAutoKeySector.SetCurSel(0);
	m_ctrComAuthKeySector.SetCurSel(0);

	m_ctrComAuthMode.AddString(_T("外部密钥A认证"));
	m_ctrComAuthMode.AddString(_T("外部密钥B认证"));
	m_ctrComAuthMode.AddString(_T("E2密钥A认证"));
	m_ctrComAuthMode.AddString(_T("E2密钥B认证"));
	m_ctrComAuthMode.SetCurSel(0);

	m_hexData.SetEndian(TRUE);
	m_hexData.SetMaxDataLen(16);
	m_hexData.SetData(ucaKey, 16);
	m_hexAuthKey.SetEndian(TRUE);
	m_hexAuthKey.SetMaxDataLen(6);
	m_hexAuthKey.SetData(ucaKey, 6);
	GetDlgItem(IDC_EDIT_AUTH_KEY)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_AUTH_KEY_SECTOR)->EnableWindow(FALSE);

	for (i = 0; i < 40; i++)
	{
		str.Format(_T("第%d扇区"), i);
		m_ctrComAutoSector.AddString(str);
		m_ctrComAuthSector.AddString(str);
	}
	m_ctrComAutoSector.SetCurSel(1);
	m_ctrComAuthSector.SetCurSel(1);

	if (32 > m_ctrComAutoSector.GetCurSel())
		j = 3;
	else
		j = 15;

	for (i = 0; i < j; i++)
	{
		str.Format(_T("从%d块开始"), i);
		m_ctrComAutoBNr.AddString(str);
		str.Format(_T("第%d块"), i);
		m_ctrComSBlock.AddString(str);
		m_ctrComDBlock.AddString(str);
		m_ctrComBlock.AddString(str);
	}
	m_ctrComBlock.AddString("密钥配置块");
	m_ctrComAutoBNr.SetCurSel(0);
	m_ctrComSBlock.SetCurSel(0);
	m_ctrComDBlock.SetCurSel(0);
	m_ctrComBlock.SetCurSel(0);

	for (i = 1; i < 4; i++)
	{
		str.Format(_T("共%d块"), i);
		m_ctrComAutoExt.AddString(str);
	}
	m_ctrComAutoExt.SetCurSel(0);

	m_bsUID.SetEditMode(EDIT_MODE_STR | EDIT_MODE_HEX);
	m_bsUID.SetEndian(FALSE);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CZLG522S_Mifare::OnSelchangeComboAuthMode() 
{
	// TODO: Add your control notification handler code here
	unsigned int m_uiAuthMode;
	
	UpdateData(TRUE);
	m_uiAuthMode = m_ctrComAuthMode.GetCurSel();
	if ((0 == m_uiAuthMode) || (1 == m_uiAuthMode))
	{
		GetDlgItem(IDC_COMBO_AUTH_KEY_SECTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTH_KEY)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_AUTH_KEY_SECTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AUTH_KEY)->EnableWindow(FALSE);
	}	
}
void CZLG522S_Mifare::OnSelchangeComboAuthSector() 
{
	// TODO: Add your control notification handler code here
	int iSector, iBlock, iSBlock, iDBlock;
	int i, iMaxBlockNo;
	CString str;
	
	iSBlock = m_ctrComSBlock.GetCurSel();
	iDBlock = m_ctrComDBlock.GetCurSel();
	iBlock  = m_ctrComBlock.GetCurSel();
	UpdateData(TRUE);
	
	iSector = m_ctrComAuthSector.GetCurSel();
	if (32 > iSector)
		iMaxBlockNo = 3;
	else
		iMaxBlockNo = 15;

	m_ctrComSBlock.ResetContent();
	m_ctrComDBlock.ResetContent();
	for (i = 0; i < iMaxBlockNo; i++)
	{
		str.Format(_T("第%d块"), i);
		m_ctrComSBlock.AddString(str);
		m_ctrComDBlock.AddString(str);
		m_ctrComBlock.AddString(str);
	}
	m_ctrComBlock.AddString("密钥配置块");

	m_ctrComSBlock.SetCurSel((iSBlock < iMaxBlockNo) ? iSBlock : (iMaxBlockNo -1));
	m_ctrComDBlock.SetCurSel((iDBlock < iMaxBlockNo) ? iDBlock : (iMaxBlockNo -1));

	m_ctrComBlock.SetCurSel((iBlock < iMaxBlockNo) ? iBlock : (iMaxBlockNo -1));
	UpdateData(FALSE);
}


void CZLG522S_Mifare::OnSelchangeComboAutoSector() 
{
	// TODO: Add your control notification handler code here
	int iAutoSector, iAutoBlock;
	int i, iMaxBlockNo;
	CString str;
	
	iAutoBlock  = m_ctrComAutoBNr.GetCurSel();
	UpdateData(TRUE);
	
	iAutoSector = m_ctrComAutoSector.GetCurSel();
	if (32 > iAutoSector)
		iMaxBlockNo = 3;
	else
		iMaxBlockNo = 15;

	m_ctrComAutoBNr.ResetContent();
	for (i = 0; i < iMaxBlockNo; i++)
	{
		str.Format(_T("从%d块开始"), i);
		m_ctrComAutoBNr.AddString(str);
	}
	m_ctrComAutoBNr.SetCurSel((iAutoBlock < iMaxBlockNo) ? iAutoBlock : (iMaxBlockNo -1));
	UpdateData(FALSE);	
}

void CZLG522S_Mifare::OnSelchangeComboAutoAuthMode() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	switch (m_ctrComAutoAuthMode.GetCurSel())
	{
	case 0:
	case 1:
		GetDlgItem(IDC_COMBO_AUTO_KEY_SECTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_KEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_AUTO_SECTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_AUTO_BNR)->EnableWindow(TRUE);
		break;

	case 2:
	case 3:
		GetDlgItem(IDC_COMBO_AUTO_KEY_SECTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_AUTO_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_AUTO_SECTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_AUTO_BNR)->EnableWindow(TRUE);
		break;

	default:
		GetDlgItem(IDC_COMBO_AUTO_KEY_SECTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_AUTO_KEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_AUTO_SECTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_AUTO_BNR)->EnableWindow(FALSE);
		break;	
	}
}

void CZLG522S_Mifare::ShowStatus(LPCTSTR lpszText, unsigned int uiStatus)
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

void CZLG522S_Mifare::OnButtonRequest() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucReqCode;
	unsigned char ucaATQ[2];

	m_strATQ = _T("");
	m_strUID = _T("");
	m_strSAK = _T("");
	ucReqCode = (m_ctrComReqCode.GetCurSel() == 0) ? ISO14443_3A_REQIDL : ISO14443_3A_REQALL;
	Status = ZLG522S_PiccRequest(m_hCtrl, ucReqCode, ucaATQ);
	if (STATUS_SUCCESS == Status)
		m_strATQ.Format(_T("%02X%02X"), ucaATQ[1], ucaATQ[0]);
	UpdateData(FALSE);
	ShowStatus(_T("请求卡"), Status);
}

void CZLG522S_Mifare::OnButtonAnti() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucaUID[4];
	
	m_strUID = _T("");
	m_strSAK = _T("");
	Status = ZLG522S_PiccAnticoll(m_hCtrl, 1, SELECT_LEVEL_1, 0, ucaUID);
	if (STATUS_SUCCESS == Status)
		m_strUID.Format(_T("%02X%02X%02X%02X"), ucaUID[3], ucaUID[2], ucaUID[1], ucaUID[0]);
	UpdateData(FALSE);
	ShowStatus(_T("防碰撞"), Status);
}

void CZLG522S_Mifare::OnButtonSelect() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucaUID[4];
	unsigned char ucSAK;
	unsigned int nLen;
	CString str;

	nLen = m_bsUID.GetData(ucaUID, 4);
	if (4 == nLen)
	{
		Status = ZLG522S_PiccSelect(m_hCtrl, SELECT_LEVEL_1, ucaUID, &ucSAK);
		if (STATUS_SUCCESS == Status)
			m_strSAK.Format(_T("%02X"), ucSAK);
		UpdateData(FALSE);
	}
	else
		Status = STATUS_USER_DATA_LEN_ERR;
	ShowStatus(_T("选择卡"), Status);
}


void CZLG522S_Mifare::OnButtonActi() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucReqCode;
	CString str;
	ACTIVEPARAA TypeAInfo;
	
	m_strATQ = _T("");
	m_strUID = _T("");
	m_strSAK = _T("");
	ucReqCode = (m_ctrComReqCode.GetCurSel() == 0) ? ISO14443_3A_REQIDL : ISO14443_3A_REQALL;
	Status = ZLG522S_PiccActivateTypeA(m_hCtrl, 1, ucReqCode, &TypeAInfo);
	if (STATUS_SUCCESS == Status)
	{
		m_strATQ.Format(_T("%02X%02X"), TypeAInfo.ATQ[1], TypeAInfo.ATQ[0]);
		for (int i = TypeAInfo.UIDLen; i != 0; i--)
		{
			str.Format(_T("%02X"), TypeAInfo.UID[i-1]);
			m_strUID += str;
		}
		m_strSAK.Format(_T("%02X"), TypeAInfo.SAK);
	}
	UpdateData(FALSE);
	ShowStatus(_T("A型卡激活"), Status);
}

void CZLG522S_Mifare::OnButtonAuth() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int uiBlock;
	unsigned int uiAuthMode;
	unsigned char ucaKey[16];
	unsigned char Status;
	CString str;

	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiBlock		= m_ctrComBlock.GetCurSel();
	if (32 > uiSector)
		uiBlock = uiSector * 4 + uiBlock;
	else
		uiBlock = (uiSector - 32) * 16 + uiBlock;

	switch(m_ctrComAuthMode.GetCurSel())
	{
	case 1:
		uiAuthMode	= KEY_SOURCE_EXT | KEY_TYPE_B;
		str = _T("外部密钥B认证");
		break;

	case 2:
		uiAuthMode	= KEY_SOURCE_E2 | KEY_TYPE_A;
		str = _T("E2密钥B认证");
		break;

	case 3:
		uiAuthMode	= KEY_SOURCE_E2 | KEY_TYPE_B;
		str = _T("E2密钥B认证");
		break;
		
	default:
		uiAuthMode	= KEY_SOURCE_EXT | KEY_TYPE_A;
		str = _T("外部密钥A认证");
	}

	if (uiAuthMode & KEY_SOURCE_E2)
		ucaKey[0] = (unsigned char)(m_ctrComAuthKeySector.GetCurSel());
	else
		m_hexAuthKey.GetData(ucaKey, 6);

	Status = ZLG522S_MF1Auth(m_hCtrl, (unsigned char)uiAuthMode, (unsigned char)uiBlock, ucaKey);
	ShowStatus(str, Status);
}

void CZLG522S_Mifare::OnButtonRead() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int i, uiBlock;
	unsigned char ucaBuf[16];
	unsigned char Status;
	CString str;
	
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiBlock		= m_ctrComBlock.GetCurSel();
	if (32 > uiSector)
		i = uiSector * 4 + uiBlock;
	else
		i = (uiSector - 32) * 16 + uiBlock;
	
	Status = ZLG522S_MF1Read(m_hCtrl, (unsigned char)i, ucaBuf);
	if (STATUS_SUCCESS != Status)
		memset(ucaBuf, 0xFF, 16);
	m_hexData.SetData(ucaBuf, 16);
	str.Format(_T("读取%d扇区的第%d块"), uiSector, uiBlock);
	ShowStatus(str, Status);
}

void CZLG522S_Mifare::OnButtonWrite() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int i, uiBlock;
	unsigned char ucaBuf[16];
	unsigned char Status;
	CString str;
	
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiBlock		= m_ctrComBlock.GetCurSel();
	if (32 > uiSector)
		i = uiSector * 4 + uiBlock;
	else
		i = (uiSector - 32) * 16 + uiBlock;

	m_hexData.GetData(ucaBuf, 16);
	
	Status = ZLG522S_MF1Write(m_hCtrl, (unsigned char)i, ucaBuf);
	str.Format(_T("向%d扇区的第%d块写入"), uiSector, uiBlock);
	ShowStatus(str, Status);	
}

void CZLG522S_Mifare::OnButtonReadVal() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int i, uiSBlock;
	unsigned char Status;
	long lValue;
	CString str;
	
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiSBlock	= m_ctrComSBlock.GetCurSel();
	if (32 > uiSector)
		i = uiSector * 4 + uiSBlock;
	else
		i = (uiSector - 32) * 16 + uiSBlock;

	Status = ZLG522S_MF1ReadValue(m_hCtrl, (unsigned char)i, &lValue);
	if (STATUS_SUCCESS == Status)
		m_lValue = lValue;
	else
		m_lValue = 0;
	UpdateData(FALSE);
	str.Format(_T("从%d扇区的第%d块读出值"), uiSector, uiSBlock);
	ShowStatus(str, Status);
	
}

void CZLG522S_Mifare::OnButtonWriteVal() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int i, uiDBlock;
	unsigned char Status;
	CString str;
	
	UpdateData(TRUE);
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiDBlock	= m_ctrComDBlock.GetCurSel();
	if (32 > uiSector)
		i = uiSector * 4 + uiDBlock;
	else
		i = (uiSector - 32) * 16 + uiDBlock;

	Status = ZLG522S_MF1WriteValue(m_hCtrl, (unsigned char)i, m_lValue);
	str.Format(_T("向%d扇区的第%d块写入值"), uiSector, uiDBlock);
	ShowStatus(str, Status);
}

void CZLG522S_Mifare::OnButtonIncVal() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int uiBlock, uiSBlock, uiDBlock;
	unsigned char Status;
	CString str;
	
	UpdateData(TRUE);
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiSBlock	= m_ctrComSBlock.GetCurSel();
	uiDBlock	= m_ctrComDBlock.GetCurSel();
	if (32 > uiSector)
		uiBlock = uiSector * 4;
	else
		uiBlock = (uiSector - 32) * 16;

	Status = ZLG522S_MF1ValueOper(m_hCtrl, MIFARE_INCREMENT, (unsigned char)(uiBlock + uiSBlock),
								(unsigned char)(uiBlock + uiDBlock),m_lValue);
	str.Format(_T("第%d扇区的第%d块加上0x%08X,\r\n结果存放到第%d块"), uiSector, uiSBlock, m_lValue & 0x7FFFFFFF, uiDBlock);
	ShowStatus(str, Status);
}

void CZLG522S_Mifare::OnButtonDec() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int uiBlock, uiSBlock, uiDBlock;
	unsigned char Status;
	CString str;
	
	UpdateData(TRUE);
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiSBlock	= m_ctrComSBlock.GetCurSel();
	uiDBlock	= m_ctrComDBlock.GetCurSel();
	if (32 > uiSector)
		uiBlock = uiSector * 4;
	else
		uiBlock = (uiSector - 32) * 16 + 128;

	Status = ZLG522S_MF1ValueOper(m_hCtrl, MIFARE_DECREMENT, (unsigned char)(uiBlock + uiSBlock),
								(unsigned char)(uiBlock + uiDBlock),m_lValue);
	str.Format(_T("第%d扇区的第%d块减去0x%08X,\r\n结果存放到第%d块"), uiSector, uiSBlock, m_lValue & 0x7FFFFFFF, uiDBlock);
	ShowStatus(str, Status);	
}

void CZLG522S_Mifare::OnButtonRes() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiSector;
	unsigned int uiBlock, uiSBlock, uiDBlock;
	unsigned char Status;
	CString str;
	
	UpdateData(TRUE);
	uiSector	= m_ctrComAuthSector.GetCurSel();
	uiSBlock	= m_ctrComSBlock.GetCurSel();
	uiDBlock	= m_ctrComDBlock.GetCurSel();
	if (32 > uiSector)
		uiBlock = uiSector * 4;
	else
		uiBlock = (uiSector - 32) * 16;

	Status = ZLG522S_MF1ValueOper(m_hCtrl, MIFARE_RESTORE, (unsigned char)(uiBlock + uiSBlock),
								(unsigned char)(uiBlock + uiDBlock),m_lValue);
	str.Format(_T("将第%d扇区的第%d块的值复制到第%d块"), uiSector, uiSBlock, uiDBlock);
	ShowStatus(str, Status);	
}

void CZLG522S_Mifare::OnButtonAutoSetPara() 
{
	// TODO: Add your control notification handler code here
	AUTO_DETECT_PAR AutoDetectPara;
	unsigned int uiAutoSector, uiAutoBlock;
	unsigned char Status;
	
	UpdateData(TRUE);
	AutoDetectPara.ADMode = 0;
	if (m_bAutoSend)
		AutoDetectPara.ADMode |= 0x01;
	if (m_bAutoInit)
		AutoDetectPara.ADMode |= 0x02;
	if (m_bAutoCont)
		AutoDetectPara.ADMode |= 0x04;
	if (m_bAutoHalt)
		AutoDetectPara.ADMode |= 0x08;
	AutoDetectPara.TXMode = (unsigned char)(m_ctrComAutoTxMode.GetCurSel() & 0x03);
	AutoDetectPara.ReqCode = (0 == m_ctrComAutoReqCode.GetCurSel()) ? ISO14443_3A_REQIDL : ISO14443_3A_REQALL;
	switch(m_ctrComAutoAuthMode.GetCurSel())
	{
	case 0:
		AutoDetectPara.AuthMode = KEY_SOURCE_EXT | KEY_TYPE_A;
		break;
	case 1:
		AutoDetectPara.AuthMode = KEY_SOURCE_EXT | KEY_TYPE_B;
		break;
	case 2:
		AutoDetectPara.AuthMode = KEY_SOURCE_E2 | KEY_TYPE_A;
		break;
	case 3:
		AutoDetectPara.AuthMode = KEY_SOURCE_E2 | KEY_TYPE_B;
		break;
	default:
		AutoDetectPara.AuthMode = 0;
	}

	if (AutoDetectPara.AuthMode & KEY_SOURCE_E2)
		AutoDetectPara.Key[0] = (unsigned char)(m_ctrComAutoKeySector.GetCurSel() & 0x0F);
	else
	{
		if (m_iAutoKeyType)
			m_hexAutoKey.GetData(AutoDetectPara.Key, 16);
		else
		{
			m_hexAutoKey.GetData(AutoDetectPara.Key, 6);
			memcpy(&AutoDetectPara.Key[6], AutoDetectPara.Key, 6);
			memcpy(&AutoDetectPara.Key[12], AutoDetectPara.Key, 4);
		}
	}

	uiAutoSector	= m_ctrComAutoSector.GetCurSel();
	uiAutoBlock		= m_ctrComAutoBNr.GetCurSel();
	if (32 > uiAutoSector)
		uiAutoBlock = uiAutoSector * 4 + uiAutoBlock;
	else
		uiAutoBlock = (uiAutoSector - 32) * 16 + uiAutoBlock + 128;
	AutoDetectPara.BNr = (unsigned char)uiAutoBlock;
	AutoDetectPara.Ext = (unsigned char)(m_ctrComAutoExt.GetCurSel() + 1);
	
	Status = ZLG522S_SetAutoDetectPar(m_hCtrl, &AutoDetectPara);
	if (STATUS_SUCCESS == Status)
	{
		CAutoDetectRes AutoDetectRes;
		AutoDetectRes.m_hCtrl = m_hCtrl;
		memcpy(&AutoDetectRes.AutoDetectPara, &AutoDetectPara, sizeof(AutoDetectPara));
		AutoDetectRes.DoModal();
	}
	else
		ShowStatus("设置自动检测参数", Status);
}

