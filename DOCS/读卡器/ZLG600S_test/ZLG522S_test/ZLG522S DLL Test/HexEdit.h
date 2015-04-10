#if !defined(AFX_HEXEDIT_H__41C5E8B8_E33C_4437_9030_7437F5BED844__INCLUDED_)
#define AFX_HEXEDIT_H__41C5E8B8_E33C_4437_9030_7437F5BED844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HexEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHexEdit window

class CHexEdit : public CEdit
{
// Construction
public:
	CHexEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetEndian(bool bEndian);
	void SetMaxDataLen(UINT nDataLen);
	bool GetData(void *pData, UINT nLength);
	bool SetData(const void *pData, UINT nLength);
	virtual ~CHexEdit();
	virtual char SetFillChar(char cFill = ' ');

	// Generated message map functions
protected:
	char m_cFillChar;
	//{{AFX_MSG(CHexEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnPaste(UINT, LONG);
	DECLARE_MESSAGE_MAP()


private:
	void SelStartChar();
	bool m_bEndian;		// 数据显示的字节顺序，false为小端，true为大端
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEXEDIT_H__41C5E8B8_E33C_4437_9030_7437F5BED844__INCLUDED_)
