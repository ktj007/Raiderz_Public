#ifndef _G_CONFIG_ARG_H
#define _G_CONFIG_ARG_H

#include "MTypes.h"
#include "GDef.h"
#include "GFrameworkLib.h"

/// 실행파일 커맨드 인자 설정
class GFRAMEWORK_API GConfig_Arg
{
public:
	bool		bAutoTestActive;
	wstring		strAutoTestType;
	float		fLimitFrame;
	int			nRepeat;
	DWORD		dwTime;	
	int			nSpeed;

	GAITestType	nAITestType;
	GCommandRecordingType nCommandRecordingType;
	int			nNPCCount;
	int			nFieldID;
	int			nQPVPEventID;

	bool		bAutoStartCommandProfile;		// 커맨드 프로파일링 시작
	bool		bLimitRunTime;
	DWORD		dwRunTime;						// 동작 시간(시간 후 자동 종료)

public:
	GConfig_Arg();
	void Create(const wchar_t* szCmd);	
};

#endif//_G_CONFIG_ARG_H
