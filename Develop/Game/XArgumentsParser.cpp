#include "stdafx.h"
#include "XArgumentsParser.h"
#include "XCrashReporter.h"
#include "XApplicationConfig.h"
#include <iomanip>
#include <regex>


XArgumentsParserResult XArgumentsParser::Parse( const wchar_t* szArgs, const cml2::MCommandLine& commandLine, XGameFrameManager* pGameFrameManager )
{
	XArgumentsParserResult ret;

	// autotest 체크
	{
		//XCrashReporter::bAutoReport = commandLine.HasSwitch("auto_report_crashdump");
	}

	// 파라미터가 리플레이 파일명인지 확인한다
	if (ParseReplayFile(szArgs, ret))
	{
		return ret;
	}

#ifdef _PUBLISH

#if defined( _KOREA)
	if ( ParseLoginPmang( szArgs, ret))						return ret;
	if ( ParseLogin( szArgs, ret))							return ret;		// (테스트용) 추후 삭제 예정
#elif defined( _BUYER)
	/*wstring strArgs;
	strArgs += szArgs;
	strArgs += L" ";
	strArgs += L"218.50.2.254";
	strArgs += L" skip_select_world";	*/				// 월드선택 스킵하기 - DirectLogin
	if ( ParseLogin( szArgs, ret))				return ret;
#else
	if ( ParseLogin( szArgs, ret))							return ret;
#endif

#else

	if ( ParseLogin( szArgs, ret))							return ret;
	if ( ParseFrameName( szArgs, pGameFrameManager, ret))	return ret;

#endif

	return ret;
}

wstring XArgumentsParser::ParseFileNameFromArguments( const wchar_t* szArgs )
{
	wchar_t szFileName[MAX_PATH] = L"";
	size_t nLength;

	// 따옴표 있으면 제거한다
	if(szArgs[0]==L'"') 
	{
		wcscpy_s(szFileName,szArgs+1);

		nLength = wcslen(szFileName);
		if(szFileName[nLength-1]==L'"')
		{
			szFileName[nLength-1]=0;
		}
	}
	else
	{
		wcscpy_s(szFileName,szArgs);

		wchar_t seps[] = L" ";
		wchar_t* token;
		wchar_t* nextToken;

		token = wcstok_s(szFileName, seps, &nextToken);
		if(token!=NULL)
		{
			wcscpy_s(szFileName, token);
		}
	}
	return wstring(szFileName);
}

bool XArgumentsParser::ParseReplayFile( const wchar_t* szArgs, XArgumentsParserResult& ret )
{
	wstring strFileName = ParseFileNameFromArguments(szArgs);
	size_t nLength = strFileName.length();

	if(wcsicmp(strFileName.c_str() + nLength - wcslen(FILEEXT_REPLAY),FILEEXT_REPLAY)==0)
	{
		mlog("MODE=REPLAY!!\n");

		ret.nExecuteMode = EM_REPLAY;
		ret.nStartGameFrameID = REPLAY_FRAME_ID;
		ret.strGameFrameArgs = strFileName;

		return true;
	}

	return false;
}

bool XArgumentsParser::CheckPrefix( const wchar_t* szArgs, const wchar_t* szStr )
{
	return (!wcsnicmp(szArgs, szStr, wcslen(szStr)));
}

bool XArgumentsParser::ParseLogin( const wchar_t* szArgs, XArgumentsParserResult& ret )
{
	//// 지스타 지나면 지웁니다.
	//if (CheckPrefix(szArgs, L"notgstar"))
	//{
	//	gvar.Network.bForceServerModeNormal = true;
	//}

	//if (CheckPrefix(szArgs, L"login") == false && 
	//	CheckPrefix(szArgs, L"launch") == false &&
	//	CheckPrefix(szArgs, L"notgstar") == false &&		// 지스타 지나면 지웁니다.
	//	CheckPrefix(szArgs, L"standalone") == false
	//	)
	//{
	//	return false;
	//}

	ret.nExecuteMode = EM_GAME;
	ret.nStartGameFrameID = LOGIN_FRAME_ID;

	int len = wcslen(szArgs);
	wchar_t szIP[256]=L"";
	wchar_t szUser[256]=L"", szPW[256]=L"";
	//
	if (wcslen(szArgs) > 5)
	{
		wchar_t temp[256]=L"";

		int ret = swscanf(szArgs, L"%s %s %s", szIP, szUser, szPW );

		if (ret == 1)
		{
			// IP를 목록에 추가
			wcsncpy_s(gvar.Network.szIP, szIP, _TRUNCATE);
			gvar.Network.vecLocatorIPList.push_back(szIP);
			gvar.Network.bDirectLogin = false;
			gvar.Network.bDirectSelChar = false;
		}
		else if( ret == 3 )
		{
			wcsncpy_s(gvar.Network.szIP, szIP, _TRUNCATE);
			gvar.Network.vecLocatorIPList.push_back(szIP);
			gvar.Network.bDirectLogin = true;
			gvar.Network.bDirectSelChar = false;
			wcsncpy_s(gvar.Network.szLoginID, szUser, _TRUNCATE);
			wcsncpy_s(gvar.Network.szPassword, szPW, _TRUNCATE);
		}
	}


	// 접속 서버타입 결정
	//if (CheckPrefix(szArgs, L"launch"))				// LoginServer
	//{
		gvar.Network.bLoginToGameServer = false;
	//}
	//else if (CheckPrefix(szArgs, L"standalone"))		// GameServer
	//{
	//	gvar.Network.bLoginToGameServer = true;
	//}
	//else												// Auto Detect
	//{			
	//	if (wcsicmp(szIP, L"localhost") && wcsicmp(szIP, L"127.0.0.1"))
	//	{
	//		gvar.Network.bLoginToGameServer = false;
	//	}
	//}

	return true;
}

bool XArgumentsParser::ParseFrameName( const wchar_t* szArgs, XGameFrameManager* pGameFrameManager, XArgumentsParserResult& ret )
{
#ifdef _PUBLISH
	return false;
#endif

	if (!pGameFrameManager) return false;


	// 인자를 Frame Name으로 체크해서 일치하는 프레임이 있으면 해당 프레임으로 바로 이동

	if (wcslen(szArgs) > 0)
	{
		wchar_t args[1024] = L"";
		wcscpy_s(args, szArgs);

		wchar_t* token;
		wchar_t seps[] = L" ,\t";
		token = wcstok(args, seps);
		if ((token != NULL) && (wcslen(token) > 0))
		{
			XBaseFrame* pFrame = pGameFrameManager->GetFrameFromName(token);
			if (pFrame)
			{
				int nFrameID = pFrame->GetID();

				mlog("MODE=%s\n", MLocale::ConvUTF16ToAnsi(token).c_str());
				ret.nExecuteMode = EM_CUSTOM_FRAME;
				ret.nStartGameFrameID = XGameFrameID(nFrameID);
				ret.strGameFrameArgs = szArgs;

				return true;
			}
		}
	}

	return false;
}

bool XArgumentsParser::ParseLoginPmang( const wchar_t* szArgs, XArgumentsParserResult& ret )
{
	enum TOKEN
	{
		COMMAND_LINE = 0,
		LOGINSTREAM_FRONT,
		USER_ID,
		TIME_STAMP,
		NEOWIZ_PCBID,				
		LOGINSTREAM_REAR,
		REGION_CODE,
		AGE_CODE,
		GENDER_CODE,		
		YEAR_OF_BIRTH,

		MAX_TOKEN
	};

	// -key"PmangLoginStream"
	tr1::wregex pattern(
						L"-key\""						// 피망 파라메터 시작 표시. version.ini 설정에 따라 차후 제거예정
							L"("						// LOGINSTREAM_FRONT
								L"\\d+"					// 유저시리얼
								L"\\|"
								L"([^| ]{1,12})"		// USER_ID
								L"\\|"
								L"(\\d+)"				// TIME_STAMP							
								L"\\|"
								L"(?:_|([^| ]{1,12}))"	// PC방 
								L"(?:\\|[^| ]+)*"		// 종량제, 채널링								
								L"\\|"
								L"[a-f\\d]{32}"			// MD5 해시
								L" "
							L")"
							L"("						// LOGINSTREAM_REAR
								L"(\\d{2})"				// REGION_CODE
								L"(\\d{1})"				// AGE_CODE
								L"(\\d{1})"				// GENDER_CODE
								L"(\\d{4})"				// YEAR_OF_BIRTH
							L")"
						L"\""
		);


	tr1::match_results<wstring::const_iterator> result;
	wstring strCommandLine = szArgs;

	if (!tr1::regex_match(strCommandLine, result, pattern))
	{
		return false;
	}

	if (MAX_TOKEN != result.size())
	{
		return false;
	}

	gvar.Network.PmangInfo.strLauncherParameter = result[LOGINSTREAM_FRONT];


	// StatIndex 재조합. RegionCode가 피망에서 약속된 값보다 1 큰값이 들어온다. -1 해서 다시 조립한다.
	int nRegionCode = _wtoi(wstring(result[REGION_CODE]).c_str());
	int nAgeCode = _wtoi(wstring(result[AGE_CODE]).c_str());
	int nGenderCode = _wtoi(wstring(result[GENDER_CODE]).c_str());
	int nYearOfBirth = _wtoi(wstring(result[YEAR_OF_BIRTH]).c_str());

	nRegionCode = max(0, nRegionCode - 1);

	// RegionCode가 크면, 해외(REGION_MAX)로 설정한다.
	const int REGION_MAX = 16;
	if (REGION_MAX < nRegionCode) nRegionCode = REGION_MAX;


	wchar_t szStatIndex[16];
	wsprintf(szStatIndex, L"%02d%d%d%04d", nRegionCode, nAgeCode, nGenderCode, nYearOfBirth);

	gvar.Network.PmangInfo.strStatIndex = szStatIndex;
	gvar.Network.PmangInfo.strLauncherParameter += szStatIndex;


	// PC방여부 파싱
	int nPCCafeID = _wtoi(wstring(result[NEOWIZ_PCBID]).c_str());
	gvar.Network.PmangInfo.nPCCafeID = nPCCafeID;


	// 성인확인, 파라메터에 포함된 태어난 해와 타임스템프를 기준으로 계산한다.
	time_t timestamp = _wtoi(wstring(result[TIME_STAMP]).c_str());
	tm* tmLocalTime = localtime(&timestamp);

	if (nYearOfBirth + 18 <= tmLocalTime->tm_year + 1900)
	{
		gvar.Network.PmangInfo.isAdult = true;		
	}
	else
	{
		gvar.Network.PmangInfo.isAdult = false;
	}


	// 피망 로그인 설정
	wcsncpy_s(gvar.Network.szLoginID, wstring(result[USER_ID]).c_str(), _TRUNCATE);
	gvar.Network.PmangInfo.isLoginOnPmang = true;

	ret.nExecuteMode = EM_GAME;
	ret.nStartGameFrameID = LOGIN_FRAME_ID;	

	gvar.Network.bDirectLogin = true;
	gvar.Network.bLoginToGameServer = false;


	// IP 설정
	gvar.Network.vecLocatorIPList = XApplicationConfig::m_vecIPString;
	
	if (!gvar.Network.vecLocatorIPList.empty())
	{
		// 접속 예정 IP는 목록의 첫번째 IP로 초기화.
		wstring strIP = gvar.Network.vecLocatorIPList.front();
		wcsncpy_s(gvar.Network.szIP, strIP.c_str(), _TRUNCATE);
	}


	return true;
}
