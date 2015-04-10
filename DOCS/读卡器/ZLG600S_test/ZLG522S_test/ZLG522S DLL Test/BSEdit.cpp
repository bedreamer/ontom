// BSEdit.cpp : implementation file
//

#include "stdafx.h"
#include "BSEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBSEdit

CBSEdit::CBSEdit()
	: m_cFillChar(' '), m_iEditMode(EDIT_MODE_STR), m_bEndian(true)
{
}

CBSEdit::~CBSEdit()
{
}

void CBSEdit::SetDataLen(UINT nMinDataLen, UINT nMaxDataLen)
{
	UINT nBytes;
	nBytes = (nMinDataLen > nMaxDataLen) ? nMinDataLen : nMaxDataLen;
	if (m_iEditMode == (EDIT_MODE_HEX | EDIT_MODE_STR))
		SetLimitText(nBytes*3 - 1);
	else
		SetLimitText(nBytes);
}

void CBSEdit::SetEndian(bool bEndian)
{
	m_bEndian = bEndian;
}
void CBSEdit::SetEditMode(INT iEditMode)
{
	m_iEditMode = iEditMode;
}
BEGIN_MESSAGE_MAP(CBSEdit, CEdit)
	//{{AFX_MSG_MAP(CBSEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBSEdit message handlers

void CBSEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(m_iEditMode) 
	{
// 	case EDIT_MODE_BIN: 
// 		if (('0' != nChar) && ('1' != nChar))
// 			return;
// 		break;
		
	case EDIT_MODE_DEC: 
		if(!isdigit(nChar))
			return;
		break;

	case EDIT_MODE_HEX:
		if(!isxdigit(nChar))
			return;
		if (('a' <= nChar) && ('f' >= nChar))
			nChar = nChar - 'a' + 'A';
		break;

	default:
		break;
	}
	SelStartChar();
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	SelStartChar();
}

void CBSEdit::SelStartChar()
{
	int iStartChar;
	int iEndChar;
	GetSel(iStartChar, iEndChar);
	if (EDIT_MODE_HEX == m_iEditMode)
	{
		if (iStartChar % 3 == 2)
		{
			iStartChar++;
		}
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


// 设置数据
bool CBSEdit::SetData(const void *pData, UINT nLength)
{
	const BYTE *_pData = static_cast<const BYTE *>(pData);
	CString strHexData;
	CString strSingleByte;
	int nStartIndex, nEndIndex, nStep, i;

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

	switch (m_iEditMode)
	{
	case EDIT_MODE_DEC:
		for (i = nStartIndex; i != nEndIndex; i += nStep)
		{
			strSingleByte.Format(_T("%01D"), _pData[i]);
			strHexData += strSingleByte;
		}
		break;

	case EDIT_MODE_HEX | EDIT_MODE_STR:
		for (i = nStartIndex; i != nEndIndex; i += nStep)
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
		break;

	default:
		for (i = nStartIndex; i != nEndIndex; i += nStep)
		{
			strSingleByte.Format(_T("%02X"), _pData[i]);
			strHexData += strSingleByte;
		}
	}
	SetWindowText(strHexData);
	
	return true;
}

// 获取数据
UINT CBSEdit::GetData(void *pData, UINT nLength)
{
	BYTE *_pData = static_cast<BYTE *>(pData);
	UINT nBytes;
	CString strHexData;
	CString strSingleByte;
	
	int nTextLen = GetWindowTextLength();
	nBytes = (m_iEditMode & EDIT_MODE_STR) ? ((nTextLen + 1) / 2) : ((nTextLen + 2) / 3);
	if (nBytes > nLength)
		nBytes = nLength;
	GetWindowText(strHexData.GetBufferSetLength(nTextLen + 1), nTextLen + 1);
	strHexData.ReleaseBuffer();
	
	int nStartIndex, nEndIndex, nStep;
	if (m_bEndian)
	{
		nStartIndex = 0;
		nEndIndex = nBytes;
		nStep = 1;
	}
	else
	{
		nStartIndex = nBytes - 1;
		nEndIndex = -1;
		nStep = -1;
	}
	
	for (int i = nStartIndex, k = 0; i != nEndIndex; i += nStep, k++)
	{
		if (m_iEditMode & EDIT_MODE_STR)
			strSingleByte = strHexData.Mid(2*k, 2);
		else
			strSingleByte = strHexData.Mid(3*k, 2);
		
		// 如果字符串为空，则数据填为0
		if (strSingleByte.IsEmpty())
		{
			_pData[i] = 0;
		}
		else
		{
			if(1 == strSingleByte.GetLength())
				strSingleByte = "0" + strSingleByte;
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
	
	return nBytes;
}
