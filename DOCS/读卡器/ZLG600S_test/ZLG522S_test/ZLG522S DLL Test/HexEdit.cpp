// HexEdit.cpp : implementation file
//

#include "stdafx.h"
#include "HexEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexEdit

CHexEdit::CHexEdit()
	: m_cFillChar(' '), m_bEndian(false)
{
}

CHexEdit::~CHexEdit()
{
}

void CHexEdit::SetMaxDataLen(UINT nDataLen)
{
	SetLimitText(nDataLen*3 - 1);
}


BEGIN_MESSAGE_MAP(CHexEdit, CEdit)
	//{{AFX_MSG_MAP(CHexEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PASTE, OnPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHexEdit message handlers
// 设置数据
bool CHexEdit::SetData(const void *pData, UINT nLength)
{
	const BYTE *_pData = static_cast<const BYTE *>(pData);
	CString strHexData;
	CString strSingleByte;

	int nStartIndex, nEndIndex, nStep;
	if (m_bEndian)
	{
		nStartIndex = 0;
		nEndIndex = nLength;
		nStep = 1;
	}
	else
	{
		nStartIndex = nLength - 1;
		nEndIndex = -1;
		nStep = -1;
	}

	for (int i = nStartIndex; i != nEndIndex; i += nStep)
	{
		strSingleByte.Format(_T("%02X"), _pData[i]);

		// 如果不是最后一个数据，则添加一个填充字符，用来隔开两个字节数据
		if (i != (nEndIndex - nStep))
		{
			strHexData += strSingleByte + m_cFillChar;
		}
		else
		{
			strHexData += strSingleByte;
		}
	}

	SetWindowText(strHexData);

	return true;
}

// 获取数据
bool CHexEdit::GetData(void *pData, UINT nLength)
{
	BYTE *_pData = static_cast<BYTE *>(pData);
	CString strHexData;
	CString strSingleByte;

	int nTextLen = GetWindowTextLength() + 1;
	GetWindowText(strHexData.GetBufferSetLength(nTextLen), nTextLen);
	strHexData.ReleaseBuffer();

	int nStartIndex, nEndIndex, nStep;
	if (m_bEndian)
	{
		nStartIndex = 0;
		nEndIndex = nLength;
		nStep = 1;
	}
	else
	{
		nStartIndex = nLength - 1;
		nEndIndex = -1;
		nStep = -1;
	}

	for (int i = nStartIndex, k = 0; i != nEndIndex; i += nStep, k++)
	{
		strSingleByte = strHexData.Mid(3*k, 2);

		// 如果字符串为空，则数据填为0
		if (strSingleByte.IsEmpty())
		{
			_pData[i] = 0;
		}
		else
		{
			_pData[i] = 0;
			strSingleByte.MakeUpper();

			for (int j = 0; j < 2; j++)
			{
				_pData[i] <<= 4;

				if (isdigit(strSingleByte[j]))
				{// 为数字 '0' ~ '9'
					_pData[i] |= strSingleByte[j] - '0';
				}
				else
				{// 为数字 'A' ~ 'F'
					_pData[i] |= strSingleByte[j] - 'A' + 0x0A;
				}
			}
		}
	}

	return true;
}

void CHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == ('A' - 'A' + 1))
	{// Ctrl + A
		SetSel(0, -1);
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else if (nChar == ('C' - 'A' + 1) || nChar == ('V' - 'A' + 1) )
	{// Ctrl + C & Ctrl + V
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else if (isxdigit(nChar))
	{
		SelStartChar();
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		SelStartChar();
	}
}

void CHexEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != VK_DELETE)
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CHexEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != VK_DELETE)
	{
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

char CHexEdit::SetFillChar(char cFill)
{
	char cOldChar = m_cFillChar;
	m_cFillChar = cFill;

	return cOldChar;
}

void CHexEdit::SetEndian(bool bEndian)
{
	m_bEndian = bEndian;
}

void CHexEdit::OnPaste(UINT, LONG)
{
	OpenClipboard();
	
	HANDLE hClip = GetClipboardData(CF_TEXT);
	if(hClip)
	{
		BYTE *pBuf = (BYTE*) GlobalLock(hClip);
		DWORD nSize = GlobalSize(hClip);
		
		for (DWORD i = 0; i < nSize - 1; i++)
		{
			SendMessage(WM_CHAR, WPARAM(pBuf[i]), MAKELONG(0, 0));
		}
		
		GlobalUnlock(hClip);
	}
	
	CloseClipboard();
}

void CHexEdit::SelStartChar()
{
	int iStartChar;
	int iEndChar;
	GetSel(iStartChar, iEndChar);
 	if (iStartChar % 3 == 2)
 	{
 		iStartChar++;
 	}

	// 如果后面还有字符，则选中后面一个字符，
	// 否则选中最后一个字符
	UINT nMaxText = GetLimitText();
 	if (static_cast<UINT>(iStartChar) < nMaxText)
	{
 		SetSel(iStartChar, iStartChar + 1, TRUE);
	}
	else
	{
 		SetSel(nMaxText - 1, -1, TRUE);
	}

}
