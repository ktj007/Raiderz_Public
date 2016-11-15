// HeightMapRS3.h : HeightMapRS3 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호


// CHeightMapRS3App:
// 이 클래스의 구현에 대해서는 HeightMapRS3.cpp을 참조하십시오.
//

class CHeightMapRS3App : public CWinApp
{
public:
	CHeightMapRS3App();


// 재정의
public:
	virtual BOOL InitInstance();

// 구현

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CHeightMapRS3App theApp;
