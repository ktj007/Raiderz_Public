#pragma once

#include "ServerCommonLib.h"
#include "MCrashDump.h"
#include "MDebug.h"
#include <signal.h>
#include "MTstring.h"

class SCommandCenter;

/// 어플리케이션 초기화등에 도움주는 헬퍼 클래스
class SCOMMON_API SAppHelper
{
public:
	/// Current Directory를 맞춤
	static void SetupCurrentDirectory();

	/// 로그 초기화
	static void InitLog(const wchar_t* szFileName);

	/// 크래쉬 덤프 초기화
	static void InitCrashDump(const wchar_t* szProgName, bool bFullMemoryDump, MCrashDump::CrashDumpHandler* handler = NULL, wstring strServerIP = L"icecream", int nServerPort=21);

	static wstring MakeCurrentDateTimeString();
	static wstring MakeCurrentDateTimeStringForFileName();

	static size_t CalcMemorySize( DWORD processID );
	static float OutputCurrentMemory();

	static void PostLogOutputCommand(SCommandCenter* pCommandCenter, MUID uidServer, const wchar_t* szText, MLogLevel nLevel);

	enum ConsoleColor
	{
		SConsoleColor_Default,
		SConsoleColor_White,
		SConsoleColor_Red,
		SConsoleColor_Green,
		SConsoleColor_Yellow,
		SConsoleColor_Blue,

		SConsoleColor_Background_Red
	};

	// 콘솔에서만 사용 가능 ----------
	static void ColoredPrintf(ConsoleColor color, const wchar_t* szText);
	static void OutputLogToConsole(MLogLevel nLevel, const wchar_t* szText);


	static int CatchTerminationSignal(void handler(int));
};