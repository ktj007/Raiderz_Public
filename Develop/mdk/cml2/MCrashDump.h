#ifndef _MCRASHDUMP_H
#define _MCRASHDUMP_H

#include "RLib.h"
#include "MDebug.h"
#include "Shlwapi.h"
#include <dbghelp.h>


// MCrashDump를 사용하려면 dbghelp.dll이 필요하다.

class CML2_API MCrashDump
{
public:
	typedef void (CrashDumpHandler)	(const char*);
private:
	static MINIDUMP_TYPE m_nDumpType;
	static char m_szFileName[128];
	static string m_strUserData;

	static CrashDumpHandler*	m_pHandlerFunc;

	static char* GetFaultReason(_EXCEPTION_POINTERS* pExPtrs);
public:
	static bool Init(const char* szFileName=NULL, const char* szUserData=NULL, bool bFullMemoryDump=false, CrashDumpHandler* pFunction=NULL);
	static LONG WINAPI TopLevelExceptionFilter(_EXCEPTION_POINTERS *pException);

	static void	GetDefaultDumpFileName(char pszFileName[]);
	static bool BuildCrashDump( const char* pszDumpFileName=NULL, _EXCEPTION_POINTERS* pException=NULL );
	//static void SnapMemoryDump();		// 어느 순간 메모리 덤프를 뜨고 싶을 때 호출
};

#endif