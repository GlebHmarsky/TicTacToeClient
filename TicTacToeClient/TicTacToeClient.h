
// TicTacToeClient.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTicTacToeClientApp:
// See TicTacToeClient.cpp for the implementation of this class
//

class CTicTacToeClientApp : public CWinApp
{
public:
	CTicTacToeClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTicTacToeClientApp theApp;
