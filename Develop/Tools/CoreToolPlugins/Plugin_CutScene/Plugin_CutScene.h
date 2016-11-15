// Plugin_CutScene.h : Plugin_CutScene DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CPlugin_CutSceneApp
// 이 클래스의 구현을 보려면 Plugin_CutScene.cpp를 참조하십시오.
//

class CPlugin_CutSceneApp : public CWinApp
{
public:
	CPlugin_CutSceneApp();

	// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CPlugin_CutSceneApp theApp;