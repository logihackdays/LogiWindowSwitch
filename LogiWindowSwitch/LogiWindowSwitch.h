
// LogiWindowSwitch.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CLogiWindowSwitchApp: 
// �аѾ\��@�����O�� LogiWindowSwitch.cpp
//

class CLogiWindowSwitchApp : public CWinApp
{
public:
	CLogiWindowSwitchApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CLogiWindowSwitchApp theApp;
