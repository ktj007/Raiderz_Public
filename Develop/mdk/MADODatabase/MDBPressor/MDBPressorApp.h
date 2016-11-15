// MDBPressor.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// MDBPressorApp:
// 이 클래스의 구현에 대해서는 MDBPressor.cpp을 참조하십시오.
//

class MDBPressorDlg;

class MDBPressorApp : public CWinApp
{
public:
	MDBPressorApp();

// 재정의입니다.
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	virtual int		Run();

// 구현입니다.
	void			HeartBeat();
	BOOL			IsShutdown()	{ return m_bShutdown;	}
	void			Shutdown()		{ m_bShutdown = TRUE;	}

	DECLARE_MESSAGE_MAP()

private:
	MDBPressorDlg*	m_dlg;
	BOOL			m_bShutdown;
};

extern MDBPressorApp theApp;