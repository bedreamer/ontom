#if !defined(AFX_ZBSEDIT_H__8B4162A5_702F_47D0_BCED_CE351F87F0D4__INCLUDED_)
#define AFX_ZBSEDIT_H__8B4162A5_702F_47D0_BCED_CE351F87F0D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZBSEdit.h : header file
//

//#define EDIT_MODE_BIN				0x01		//
#define EDIT_MODE_DEC				0x02		//
#define EDIT_MODE_HEX				0x03		//

#define EDIT_MODE_STR				0x10		// 字符串方式
//#define EDIT_MODE_BYTE			0x00		// 字节方式
/////////////////////////////////////////////////////////////////////////////
// CZBSEdit window

class CBSEdit : public CEdit
{
// Construction
public:
	CBSEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBSEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetEndian(bool bEndian);
	void SetEditMode(INT iEditMode);
	void SetDataLen(UINT nMinDataLen, UINT nMaxDataLen);
	bool SetData(const void *pData, UINT nLength);
	UINT GetData(void *pData, UINT nLength);

	virtual ~CBSEdit();
	

	// Generated message map functions
protected:
	char m_cFillChar;

	//{{AFX_MSG(CBSEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	bool m_bEndian;		// 数据显示的字节顺序，false为小端，true为大端
	INT m_iEditMode;
	void SelStartChar();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZBSEDIT_H__8B4162A5_702F_47D0_BCED_CE351F87F0D4__INCLUDED_)
