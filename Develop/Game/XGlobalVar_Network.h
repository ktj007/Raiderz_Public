#pragma once

#include "CTransData.h"
#include "CSDef.h"

/// 피망 로그인 정보
struct XPmangLoginInfo
{
	bool				isLoginOnPmang;					// 피망에서 로그인하는지 여부
	wstring				strLauncherParameter;			// 런처로 전달받은 로그인 문자열
	wstring				strStatIndex;					// 런처로 받은 통계 코드		
	bool				isAdult;						// 18세 이상여부
	int					nPCCafeID;

	XPmangLoginInfo() : isLoginOnPmang(false), isAdult(false), nPCCafeID(0) 
	{
	}
};


/// 전역적으로 사용하는 네트워크 정보
struct GlobalVar_Network
{
	bool				bDirectLogin;					// 바로 로긴할껀지
	bool				bWaitDirectLogin;				// 바로 로그인 대기상태 - 에러팝업이 떠있는 동안은 대기
	bool				bIsAlreadyDirectLogined;		// 이미 바로 로그인을 한번 했는지 체크
	bool				bDirectSelChar;					// 바로 캐릭터 선택할건지
	wchar_t				szLoginID[64];					// 바로 로긴할 때 사용할 로긴 ID
	wchar_t				szPassword[64];					// 바로 로긴할 때 사용할 패스워드
	wchar_t				szIP[64];						// 바로 로긴할 때 사용할 IP	- 월드 IP
	bool				bLoginToGameServer;				// 로그인 서버로 연결하지 않고 게임서버로 연결할 지 여부

	wchar_t				szStartZone[128];				// 바로 로긴할 때 시작 존
	vec3				vStartPos;						// 바로 로긴할 때 시작 위치
	vec3				vStartDir;						// 바로 로긴할 때 시작 방향
	int					nDirectLoginFieldID;			// 바로 로긴할 때 시작할 필드 ID
	wstring				strSelChar;						// 바로 로긴할 때 선택할 캐릭터 이름
	wstring				szWorldName;					// 바로 로긴할 때 선택할 월드 이름

	CSServerMode		nServerMode;					// 게임 서버 모드
	bool				bForceServerModeNormal;			// 지스타 지나면 지웁니다.

	XPmangLoginInfo		PmangInfo;						// 피망 로그인 정보
	
	vector<wstring>		vecLocatorIPList;				// 로케이터 IP 목록
	vector<CSWorldInfo>	vecWorldInfo;					// 월드 목록
	

	GlobalVar_Network()
	{
		bDirectLogin = false;
		bWaitDirectLogin = false;
		bIsAlreadyDirectLogined = false;

		bDirectSelChar = false;
		szLoginID[0] = szPassword[0] = szIP[0] = 0;
		bLoginToGameServer = true;
		nDirectLoginFieldID = -1;	
		szStartZone[0] = 0;
		nServerMode = SERVER_MODE_NORMAL;
		bForceServerModeNormal = false;
	}

	bool IsExistDirectLoginFieldInfo()
	{
		return (0 < nDirectLoginFieldID);
	}

	void ResetDirectLoginFieldInfo()
	{
		nDirectLoginFieldID = -1;
		vStartPos = vec3(0,0,0);
	}

	bool IsServerModeExpo()
	{
		return (nServerMode == SERVER_MODE_EXPO);
	}
};
