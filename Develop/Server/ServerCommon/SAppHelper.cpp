#include "stdafx.h"
#include "SAppHelper.h"
#include "MDebug.h"
#include <shlwapi.h>
#include "SCrashReporter.h"
#include "SCommandCenter.h"
#include "SCommandTable_App.h"
#include "MLocale.h"
#include <psapi.h>
#include <stdlib.h>

#pragma comment( lib, "psapi")

void SAppHelper::SetupCurrentDirectory()
{
	wchar_t szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}

void SAppHelper::InitLog(const wchar_t* szFileName)
{	
	MInitLog(MLOG_LEVEL_DEBUG, MLOG_DEBUG | MLOG_FILE, NULL, MLocale::ConvTCHARToAnsi(szFileName).c_str());

	MLogLevel nLogLevel = MLOG_LEVEL_INFORMATION;

#ifndef _PUBLISH
	nLogLevel = MLOG_LEVEL_DEBUG;
#	ifndef _DEBUG
	nLogLevel = MLOG_LEVEL_INFORMATION;
#	endif

	unsigned long int nOutputFlags = MLOG_DEBUG | MLOG_PROG | MLOG_FILE;

	// Prog Output은 나중에 세팅한다.
	MInitLog(nLogLevel, nOutputFlags, NULL, MLocale::ConvTCHARToAnsi(szFileName).c_str());
#endif
}

void SAppHelper::InitCrashDump( const wchar_t* szProgName, bool bFullMemoryDump, MCrashDump::CrashDumpHandler* handler, wstring strServerIP/*="icecream"*/, int nServerPort/*=21*/ )
{
	SCrashReporter::Init(szProgName, true, strServerIP, nServerPort);

	if (!handler)
	{
		handler = &SCrashReporter::CrashDumpHandler;
	}

	MCrashDump::Init(NULL, NULL, bFullMemoryDump, handler);
}

void SAppHelper::ColoredPrintf( ConsoleColor color, const wchar_t* szText )
{
	WORD wAttributes = 0;

	switch (color) 
	{
	case SConsoleColor_White:			wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;	break;
	case SConsoleColor_Red:				wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED;	break;
	case SConsoleColor_Green:			wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN;	break;
	case SConsoleColor_Yellow:			wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;	break;
	case SConsoleColor_Background_Red:	wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_RED;	break;
	default:							wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
	}

	const HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(stdout_handle, wAttributes);

	DWORD writtenCount;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), szText, wcslen(szText), &writtenCount, NULL);
}

wstring SAppHelper::MakeCurrentDateTimeString()
{	
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);

	wchar_t szTime[64] = L"";

	swprintf_s(szTime, L"[%02d/%02d %02d:%02d:%02d]"
		, stCurTime.wMonth
		, stCurTime.wDay
		, stCurTime.wHour
		, stCurTime.wMinute
		, stCurTime.wSecond
		);
	
	return wstring(szTime);
}

void SAppHelper::OutputLogToConsole( MLogLevel nLevel, const wchar_t* szText )
{
	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			SAppHelper::ColoredPrintf(SAppHelper::SConsoleColor_Default, szText); break;
	case MLOG_LEVEL_INFORMATION:	SAppHelper::ColoredPrintf(SAppHelper::SConsoleColor_White, szText); break;
	case MLOG_LEVEL_WARNING:		SAppHelper::ColoredPrintf(SAppHelper::SConsoleColor_Yellow, szText); break;
	case MLOG_LEVEL_ERROR:
		{
			SAppHelper::ColoredPrintf(SAppHelper::SConsoleColor_Background_Red, szText);
		}
		break;
	default:
		SAppHelper::ColoredPrintf(SAppHelper::SConsoleColor_Default, szText); break;
	};
}

void SAppHelper::PostLogOutputCommand( SCommandCenter* pCommandCenter, MUID uidServer, const wchar_t* szText, MLogLevel nLevel )
{
	if (NULL == pCommandCenter->GetNetServer()) return;

	MCommand* pNewCmd = pCommandCenter->NewCommand(MAPP_LOG_OUT_PUT);
	pNewCmd->AddParameter(NEW_WSTR(szText));
	pNewCmd->AddParameter(NEW_INT(nLevel));

	pNewCmd->SetReceiverUID(uidServer);
	pCommandCenter->PostCommand(pNewCmd);
}

wstring SAppHelper::MakeCurrentDateTimeStringForFileName()
{
	wstring strRet;

	time_t currtime;
	time(&currtime);
	struct tm* pTM = localtime(&currtime);

	wchar_t    szBuffer[_MAX_PATH] = L"";
	swprintf_s(szBuffer, L"_%04d-%02d-%02d_%02d-%02d-%02d",
		pTM->tm_year + 1900,
		pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, pTM->tm_sec);

	strRet = szBuffer;

	return strRet;
}

size_t SAppHelper::CalcMemorySize( DWORD processID )
{
	size_t nRet = 0;

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	// Print the process identifier.

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID );
	if (NULL == hProcess)
		return 0;

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		nRet = pmc.WorkingSetSize;
	}

	CloseHandle( hProcess );

	return nRet;
}

float SAppHelper::OutputCurrentMemory()
{
	size_t nCurrentByte = SAppHelper::CalcMemorySize(::GetCurrentProcessId());
	float fMByte = nCurrentByte / 1024.0f / 1024.0f;
	mlog("Current Memory = %.1fMB\n", fMByte);

	return fMByte;
}
