
// 3DEngine.h : main header file for the 3DEngine application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMy3DEngineApp:
// See 3DEngine.cpp for the implementation of this class
//

class CMy3DEngineApp : public CWinAppEx
{
public:
	CMy3DEngineApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMy3DEngineApp theApp;
