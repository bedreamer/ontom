// ZLG522S DLL Test.h : main header file for the ZLG522S DLL TEST application
//

#if !defined(AFX_ZLG522SDLLTEST_H__5E1B6A86_7363_4BB9_8B28_D925306567B9__INCLUDED_)
#define AFX_ZLG522SDLLTEST_H__5E1B6A86_7363_4BB9_8B28_D925306567B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CZLG522SDLLTestApp:
// See ZLG522S DLL Test.cpp for the implementation of this class
//

class CZLG522SDLLTestApp : public CWinApp
{
public:
	CZLG522SDLLTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZLG522SDLLTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CZLG522SDLLTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZLG522SDLLTEST_H__5E1B6A86_7363_4BB9_8B28_D925306567B9__INCLUDED_)
