// ZLG522S DLL TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZLG522S DLL Test.h"
#include "ZLG522S DLL TestDlg.h"
#include "ZLG522S.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZLG522SDLLTestDlg dialog

CZLG522SDLLTestDlg::CZLG522SDLLTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZLG522SDLLTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZLG522SDLLTestDlg)
	m_strDevInfo = _T("");
	m_iTxMode = 0;
	m_ucPauseTime = 10;
	m_ucWaitTime = 10;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZLG522SDLLTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZLG522SDLLTestDlg)
	DDX_Control(pDX, IDC_TAB_CARD_MODE, m_tTab);
	DDX_Control(pDX, IDC_EDIT_REG_VAL, m_hexRegVal);
	DDX_Control(pDX, IDC_EDIT_REG_ADR, m_hexRegAdr);
	DDX_Control(pDX, IDC_COMBO_MODULE_TYPE, m_ctrComModuleType);
	DDX_Control(pDX, IDC_COMBO_COMNO, m_ctrComComNo);
	DDX_Control(pDX, IDC_COMBO_BAUD_RATE, m_ctrComBaudRate);
	DDX_Text(pDX, IDC_EDIT_DEV_INFO, m_strDevInfo);
	DDV_MaxChars(pDX, m_strDevInfo, 16);
	DDX_Radio(pDX, IDC_RADIO_TX_MODE, m_iTxMode);
	DDX_Text(pDX, IDC_EDIT_PAUSE_TIME, m_ucPauseTime);
	DDV_MinMaxByte(pDX, m_ucPauseTime, 0, 255);
	DDX_Text(pDX, IDC_EDIT_WAIT_TIME, m_ucWaitTime);
	DDV_MinMaxByte(pDX, m_ucWaitTime, 0, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZLG522SDLLTestDlg, CDialog)
	//{{AFX_MSG_MAP(CZLG522SDLLTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_GET_DEV_INFO, OnButtonGetDevInfo)
	ON_BN_CLICKED(IDC_BUTTON_CFG_PCD, OnButtonCfgPcd)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_PCD, OnButtonClosePcd)
	ON_BN_CLICKED(IDC_BUTTON_READ_REG, OnButtonReadReg)
	ON_BN_CLICKED(IDC_BUTTON_WRATE_REG, OnButtonWrateReg)
	ON_BN_CLICKED(IDC_BUTTON_CARD_RESET, OnButtonCardReset)
	ON_BN_CLICKED(IDC_BUTTON_SET_MODULE_WORK_MODE, OnButtonSetModuleWorkMode)
	ON_BN_CLICKED(IDC_RADIO_TX_MODE, OnRadioTxMode)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CARD_MODE, OnSelchangeTabCardMode)
	ON_CBN_SELCHANGE(IDC_COMBO_MODULE_TYPE, OnSelchangeComboModuleType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZLG522SDLLTestDlg message handlers

BOOL CZLG522SDLLTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_tTab.InsertItem(0,"Mifare");
	m_tTab.InsertItem(1,"ISO14443A");
	m_tTab.InsertItem(2,"PLUS CPU");

	if (!m_tMifareTab.m_hWnd || !m_tISO14443ATab.m_hWnd ||
		!m_tPlusCPU.m_hWnd)
	{
		//关联对话框,并且将IDC_TAB控件设为父窗口
		m_tMifareTab.Create(IDD_DIALOG_MIFARE,GetDlgItem(IDC_TAB_CARD_MODE));
		m_tISO14443ATab.Create(IDD_DIALOG_ISO14443,GetDlgItem(IDC_TAB_CARD_MODE));
		m_tPlusCPU.Create(IDD_DIALOG_PLUSCPU,GetDlgItem(IDC_TAB_CARD_MODE));
		
		//获得IDC_TAB_CARD_MODE客户区大小
		CRect rs;
		m_tTab.GetClientRect(rs);
		
		//调整子对话框在父窗口中的位置
		rs.top +=19;
		rs.bottom-=0;
		rs.left+=0;
		rs.right-=0;
		
		//设置子对话框尺寸并移动到指定位置
		m_tMifareTab.MoveWindow(&rs);
		m_tISO14443ATab.MoveWindow(&rs);
		m_tPlusCPU.MoveWindow(&rs);
		
		//分别设置隐藏和显示
		// 		m_PiccTest.ShowWindow(FALSE);
		// 		m_CiccTest.ShowWindow(FALSE);
		// 		m_HardWardTest.ShowWindow(TRUE);
		// 		m_SyntheticTest.ShowWindow(FALSE);
		m_tMifareTab.ShowWindow(SW_SHOW);
		
		//设置默认的选项卡
		m_tTab.SetCurSel(0);
		
		m_tTab.BringWindowToTop();
	}

	m_ctrComModuleType.AddString(_T("ZLG500S"));
	m_ctrComModuleType.AddString(_T("ZLG522S"));
	m_ctrComModuleType.AddString(_T("ZLG522SP"));
	m_ctrComModuleType.AddString(_T("ZLG522SP_L"));
	m_ctrComModuleType.AddString(_T("ZLG522SP_EX"));
	m_ctrComModuleType.SetCurSel(1);

	m_ctrComComNo.AddString(_T("COM1"));
	m_ctrComComNo.AddString(_T("COM2"));
	m_ctrComComNo.AddString(_T("COM3"));
	m_ctrComComNo.AddString(_T("COM4"));
	m_ctrComComNo.SetCurSel(0);

	m_ctrComBaudRate.AddString(_T("9600"));
	m_ctrComBaudRate.AddString(_T("19200"));
	m_ctrComBaudRate.AddString(_T("28800"));
	m_ctrComBaudRate.AddString(_T("38400"));
	m_ctrComBaudRate.AddString(_T("57600"));
	m_ctrComBaudRate.AddString(_T("115200"));
	m_ctrComBaudRate.SetCurSel(4);

	unsigned char ucTemp = 0x00;

	m_hexRegAdr.SetMaxDataLen(1);
	m_hexRegAdr.SetEndian(TRUE);
	m_hexRegAdr.SetData(&ucTemp, 1);
	m_hexRegVal.SetMaxDataLen(1);
	m_hexRegVal.SetEndian(TRUE);
	m_hexRegVal.SetData(&ucTemp, 1);

	m_iTxMode = 0x03;
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZLG522SDLLTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZLG522SDLLTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZLG522SDLLTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CZLG522SDLLTestDlg::ShowStatus(LPCTSTR lpszText, unsigned int uiStatus)
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

void CZLG522SDLLTestDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	unsigned int uiBaudRateTab[] ={CBR_9600, CBR_19200, CBR_28800, CBR_38400, CBR_57600, CBR_115200};
	unsigned char Status;
	CString str;

	UpdateData(FALSE);
//	IDC_BUTTON_OPEN
	unsigned int uiModuleType = m_ctrComModuleType.GetCurSel();
	unsigned int uiBaudRate = uiBaudRateTab[m_ctrComBaudRate.GetCurSel()];
	m_ctrComComNo.GetLBText(m_ctrComComNo.GetCurSel(), str);

	Status = STATUS_COMM_ERROR;
	if (ZLG522S_Init(str, uiBaudRateTab[m_ctrComBaudRate.GetCurSel()], m_ctrComModuleType.GetCurSel()))
	{
		m_hCtrl = ZLG522S_Open(0x00);
		if (INVALID_HANDLE_VALUE != m_hCtrl)
		{
			Status = STATUS_SUCCESS;
			Beep(3000, 100);

		}
	}	
	ShowStatus(_T("模块通讯设置"), Status);
}

void CZLG522SDLLTestDlg::OnButtonGetDevInfo() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned int nBytes;
//	unsigned char ucaTempBuf[16];
	char szDevInfo[17];
	CString str;

	Status = ZLG522S_GetDevInfo(m_hCtrl, (unsigned char *)szDevInfo, sizeof(szDevInfo) - 1, &nBytes);
	if (STATUS_SUCCESS == Status)

		szDevInfo[nBytes] = '\0';
	else
		szDevInfo[0] = '\0';
	m_strDevInfo = szDevInfo;
	UpdateData(FALSE);
	ShowStatus(_T("获取模块固件信息"), Status);	
}


void CZLG522SDLLTestDlg::OnButtonCfgPcd() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status = ZLG522S_PcdConfig(m_hCtrl, ISO14443_TYPEA);
	ShowStatus(_T("配置读写卡芯片"), Status);	
}

void CZLG522SDLLTestDlg::OnButtonClosePcd() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status = ZLG522S_PcdClose(m_hCtrl);
	ShowStatus(_T("关闭读写卡芯片"), Status);	
	
}

void CZLG522SDLLTestDlg::OnButtonReadReg() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucRegAdr, ucRegVal;
	CString str;

	m_hexRegAdr.GetData(&ucRegAdr, 1);
	ucRegVal = 0x00;
	Status = ZLG522S_PcdReadReg(m_hCtrl, ucRegAdr, &ucRegVal);
	m_hexRegVal.SetData(&ucRegVal, 1);
	str.Format(_T("读读写卡芯片寄存器地址0x%02X"), ucRegAdr);
	ShowStatus(str, Status);
}

void CZLG522SDLLTestDlg::OnButtonWrateReg() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	unsigned char ucRegAdr, ucRegVal;
	CString str;
	
	m_hexRegAdr.GetData(&ucRegAdr, 1);
	m_hexRegVal.GetData(&ucRegVal, 1);
	Status = ZLG522S_PcdWriteReg(m_hCtrl, ucRegAdr, ucRegVal);
	str.Format(_T("向读写卡芯片寄存器地址0x%02X，\r\n写入0x%02X"), ucRegAdr, ucRegVal);
	ShowStatus(str, Status);
}

void CZLG522SDLLTestDlg::OnButtonCardReset() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;

	UpdateData(TRUE);
	Status = ZLG522S_CardReset(m_hCtrl, m_ucPauseTime, m_ucWaitTime);
	ShowStatus(_T("卡片复位"), Status);
}

void CZLG522SDLLTestDlg::OnButtonSetModuleWorkMode() 
{
	// TODO: Add your control notification handler code here
	CSetModuleMode SetModuleMode;
	if(SetModuleMode.DoModal() == IDOK)
	{

	}
}

void CZLG522SDLLTestDlg::OnRadioTxMode() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	
	Status = ZLG522S_PcdSetTX(m_hCtrl, 0x00);
	ShowStatus(_T("设置天线驱动模式（关闭）"), Status);	
}

void CZLG522SDLLTestDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	
	Status = ZLG522S_PcdSetTX(m_hCtrl, 0x01);
	ShowStatus(_T("设置天线驱动模式（TX1）"), Status);
}

void CZLG522SDLLTestDlg::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	
	Status = ZLG522S_PcdSetTX(m_hCtrl, 0x00);
	ShowStatus(_T("设置天线驱动模式（TX2）"), Status);
}

void CZLG522SDLLTestDlg::OnRadio4() 
{
	// TODO: Add your control notification handler code here
	unsigned char Status;
	
	Status = ZLG522S_PcdSetTX(m_hCtrl, 0x00);
	ShowStatus(_T("设置天线驱动模式（TX1、2）"), Status);
}


void CZLG522SDLLTestDlg::OnSelchangeTabCardMode(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	switch(m_tTab.GetCurSel())
	{
	case 0:
		m_tMifareTab.ShowWindow(SW_SHOW);
		m_tISO14443ATab.ShowWindow(SW_HIDE);
		m_tMifareTab.SetFocus();
		m_tMifareTab.m_hCtrl = m_hCtrl;
		break;
		
	case 1:
		m_tMifareTab.ShowWindow(SW_HIDE);
		m_tISO14443ATab.ShowWindow(SW_SHOW);
		m_tISO14443ATab.SetFocus();
		break;

	default:
        break;
	}
	*pResult = 0;
}

void CZLG522SDLLTestDlg::OnSelchangeComboModuleType() 
{
	// TODO: Add your control notification handler code here
	unsigned char ucaKey[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	int iModuleType;
	unsigned int nBytes;

	UpdateData(TRUE);
	m_tMifareTab.UpdateData(TRUE);	
	iModuleType = m_ctrComModuleType.GetCurSel();
	if ((MT_ZLG522SP == iModuleType) ||
		(MT_ZLG522SP_L == iModuleType) || 
		(MT_ZLG522SP_EX == iModuleType))
	{
		m_tMifareTab.GetDlgItem(IDC_COMBO_AUTO_EXT)->EnableWindow(TRUE);
		m_tMifareTab.GetDlgItem(IDC_RADIO_AUTO_AES)->EnableWindow(TRUE);
		//if (m_tMifareTab.m_iAutoKeyType)
			nBytes = 16;
		//else
		//	nBytes = 6;
		m_tMifareTab.m_hexAutoKey.SetMaxDataLen(nBytes);
		m_tMifareTab.m_hexAutoKey.SetData(ucaKey, nBytes);
	}
	else
	{
		m_tMifareTab.m_iAutoKeyType = 0;
		m_tMifareTab.GetDlgItem(IDC_COMBO_AUTO_EXT)->EnableWindow(FALSE);
		m_tMifareTab.GetDlgItem(IDC_RADIO_AUTO_AES)->EnableWindow(FALSE);
		m_tMifareTab.m_hexAutoKey.SetMaxDataLen(6);
		m_tMifareTab.m_hexAutoKey.SetData(ucaKey, 6);
	}
	m_tMifareTab.UpdateData(FALSE);	
	UpdateData(FALSE);	
}
