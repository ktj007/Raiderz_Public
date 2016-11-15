#include "stdafx.h"
#include "MCrashDump.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
	HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

MINIDUMP_TYPE MCrashDump::m_nDumpType = MiniDumpNormal;
char MCrashDump::m_szFileName[128] = "";
string MCrashDump::m_strUserData;
MCrashDump::CrashDumpHandler* MCrashDump::m_pHandlerFunc = NULL;

char* MCrashDump::GetFaultReason(_EXCEPTION_POINTERS* pExPtrs)
{
	if( NULL == pExPtrs || NULL == pExPtrs->ExceptionRecord || NULL ==  pExPtrs->ExceptionRecord->ExceptionCode )
		return "Unknown";

	if (::IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS))) 
		return "BAD EXCEPTION POINTERS";

	// 간단한 에러 코드라면 그냥 변환할 수 있다.
	//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/debug/base/exception_record_str.asp
	switch( pExPtrs->ExceptionRecord->ExceptionCode )
	{
	case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";		// 잘못된 참조
	case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";  // 정렬되지않은 데이타접근
	case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";				// 브레이크포인트에 걸렸을때
	case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";			// 하나의 명령어를 실행할때
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";	// 배열이 넘어접근
	case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";	// 정규화가 되지않는 작은값
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";		// float 0으로나눔
	case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";		// 소수를 정확하게 나타낼수없음
	case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";	// 기타예외
	case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";			// float오버플로우
	case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";		// 스택에값이 넘치거나 너무작을경우
	case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";			// 값이 작을때
	case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";		// int를 0으로 나눔
	case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";			// int오버플로우
	case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";		// 명령어를 실행 할 수 없을때
	case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";			// page를 로드 할 수 없는데 접근한경우
	case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";	// 잘못된 명령어
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";// 진행할수 없는데 계속 진행시킬 경우
	case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";			// 스택오버플로우
	case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";	// 잘못된기능(?)접근 C에서는 발생하지 않음
	case EXCEPTION_GUARD_PAGE:               return "EXCEPTION_GUARD_PAGE";				// 접근하지못하는 page에 접근
	case EXCEPTION_INVALID_HANDLE:           return "EXCEPTION_INVALID_HANDLE";			// 잘못된 핸들
	case 0xE06D7363:                         return "Microsoft C++ Exception";			// Visual C++ 오류
	default:
		break;
	}

	return "Unknown";
}

void MCrashDump::GetDefaultDumpFileName( char pszFileName[] )
{
	char szDumpFileName[MAX_PATH];
	char szFilename[MAX_PATH];
	::GetModuleFileName(NULL, szFilename, MAX_PATH);
	char* szdot = strrchr(szFilename, '.');
	::lstrcpyn(szDumpFileName, szFilename, (int)(szdot - szFilename + 1));

	SYSTEMTIME time;
	::GetLocalTime(&time);
	sprintf( pszFileName, "%d%02d%02d_%02d%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

LONG WINAPI MCrashDump::TopLevelExceptionFilter(_EXCEPTION_POINTERS *pException)
{
	char szDumpFileName[MAX_PATH];
	if (m_szFileName[0] == 0)
	{
		GetDefaultDumpFileName(szDumpFileName);
	}
	else
	{
		strcpy_s(szDumpFileName, m_szFileName);
	}

	if (!BuildCrashDump(szDumpFileName, pException))
		return EXCEPTION_CONTINUE_SEARCH;

	if ( m_pHandlerFunc )
		m_pHandlerFunc(szDumpFileName);

	return EXCEPTION_EXECUTE_HANDLER;
}


bool MCrashDump::Init(const char* szFileName, const char* szUserData, bool bFullMemoryDump, CrashDumpHandler* pFunction)
{
	::SetUnhandledExceptionFilter(TopLevelExceptionFilter);

	if (szFileName!=NULL)
	{
		strcpy_s(m_szFileName, szFileName);
	}
	if (szUserData!=NULL)
	{
		m_strUserData = szUserData;
	}

	m_nDumpType = MiniDumpNormal;
	if (bFullMemoryDump)
	{
		m_nDumpType = MiniDumpNormal;
	}

	if ( pFunction )
		m_pHandlerFunc = pFunction;

	return true;
}

bool MCrashDump::BuildCrashDump( const char* pszDumpFileName, _EXCEPTION_POINTERS* pException )
{
	char szDllHelpPath[MAX_PATH] = "";
	HMODULE hDLL = NULL;

	// 기본 저장할 파일 이름 설정
	char szDumpFileName[MAX_PATH];
	if (NULL == pszDumpFileName)
	{
		GetDefaultDumpFileName(szDumpFileName);
		pszDumpFileName = szDumpFileName;
	}

	// DLL 로딩
	if(::GetModuleFileName(NULL, szDllHelpPath, _MAX_PATH))
	{
		char *pSlash = ::strrchr(szDllHelpPath, '\\');
		if( pSlash )
		{
			::lstrcpy(pSlash + 1, "DBGHELP.DLL");
			hDLL = ::LoadLibrary(szDllHelpPath);
		}
	}

	if( NULL == hDLL || INVALID_HANDLE_VALUE == hDLL )
	{
		MessageBox(NULL, "cannot open file DBGHELP.DLL", "ERROR", MB_OK);
		return false;
	}

	// DLL에서 MiniDumpWriteDump함수 얻어오기
	MINIDUMPWRITEDUMP pMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");

	if( pMiniDumpWriteDump == NULL )
	{
		return false;
	}

	// 파일 생성
	HANDLE hFile = CreateFile(
		pszDumpFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if( INVALID_HANDLE_VALUE == hFile )	
	{
		return false;
	}

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = pException;
	eInfo.ClientPointers = FALSE;

	// 주의 //
	// 여기는 익셉션흘 핸들링하는 부분이므로 메모리 침범 익셉션 같은경우에 
	// 이곳에서 메모리 동적할당을 하면 두번째 익셉션이 나면서 종료될수 있으므로 
	// 메모리 동적할당은 피해야 합니다

	const char*	szFaultReason;
	szFaultReason = pException ? GetFaultReason(pException):"UserDump";

	MINIDUMP_USER_STREAM_INFORMATION sUserInfo; 
	MINIDUMP_USER_STREAM m_vUseData[2];

	// 추가정보
	m_vUseData[0].Type = 0;
	m_vUseData[0].Buffer = (PVOID)szFaultReason;
	m_vUseData[0].BufferSize = strlen(szFaultReason);

	m_vUseData[1].Type = 1;
	m_vUseData[1].Buffer = (PVOID)m_strUserData.c_str();
	m_vUseData[1].BufferSize = m_strUserData.size();

	sUserInfo.UserStreamCount = 2; 
	sUserInfo.UserStreamArray = m_vUseData;

	bool bDumpped = false;
	if (pMiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		m_nDumpType,
		pException ? &eInfo : NULL,
		&sUserInfo,
		NULL) )
	{
		bDumpped = true;
	}

	CloseHandle(hFile);
	FreeLibrary(hDLL);

	return bDumpped;
}

