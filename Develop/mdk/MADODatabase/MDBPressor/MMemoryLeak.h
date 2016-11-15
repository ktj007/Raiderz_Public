#ifndef _MMEMORY_LEAK_H
#define _MMEMORY_LEAK_H

/************************************************************
MODULE NAME : MemoryLeak

HISTORY : 
Date		Name		Description
2007-02-12	남기룡		
DESCRIPTION
	- MMemoryLeak.h를 인클루드하기만 하면 종료시 Leak 메세지가 출력됩니다.
	- Debug일 때에만 동작합니다.
	- http://msdn.microsoft.com/library/kor/default.asp?url=/library/KOR/vsdebug/html/_core_Using_the_Debug_Heap.asp

	- 메모리 할당 영역
	  - 스택변수 : CC CC CC CC
	  - 할당한메모리 : CD CD CD CD
	  - 삭제된메모리: DD DD DD DD
	  - 할당한메모리양끝: FD FD FD FD

*************************************************************/

#ifdef _MSC_VER
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC	// 메모리 누수 탐지를 위해서 선언해 주어야 한다.
#include <crtdbg.h>

#ifndef _CONSOLE
#include <cstdlib>
#endif

class MMemoryLeak
{
public:
	MMemoryLeak()
	{
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		// 콘솔 프로그램일 경우
#ifdef _CONSOLE

		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );


#endif	// _CONSOLE

// mfc 사용하는 프로젝트는 DEBUG_NEW 사용하세요.
#if defined (__AFX_H__)

#ifndef new
#define new DEBUG_NEW 
#endif

#else

#define DEBUG_NORMALBLOCK new(_NORMAL_BLOCK, __FILE__, __LINE__)

#ifdef new
#undef new
#endif

#define new DEBUG_NORMALBLOCK


#endif




#ifdef malloc
#undef malloc
#endif
#define malloc(s) (_malloc_dbg( s, _NORMAL_BLOCK, __FILE__, __LINE__ ))

#ifdef calloc
#undef calloc
#endif
#define calloc(c, s)      _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)

#ifdef realloc
#undef realloc
#endif
#define realloc(p, s)     _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)

#ifdef _expand
#undef _expand
#endif
#define _expand(p, s)     _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)

#ifdef free
#undef free
#endif
#define free(p)           _free_dbg(p, _NORMAL_BLOCK)

#ifdef _msize
#undef _msize
#endif
#define _msize(p)         _msize_dbg(p, _NORMAL_BLOCK)

	}

	/// 힙의 무결성을 확인. assert(MMemoryLeak::CheckMemory()); 이런식으로 사용하면 된다.
	static int CheckMemory()			
	{
		return _CrtCheckMemory();
	}

	/// 해당 메모리 할당번호로 브레이크 포인트를 설정한다.
	static void BreakAlloc(long ncrtBreakAlloc)
	{
		//_crtBreakAlloc 변수를 직접 설정해도 됨.
		_CrtSetBreakAlloc(ncrtBreakAlloc);
	}
};

static MMemoryLeak g_MemoryLeak;

#endif	// _DEBUG
#endif	// _MSC_VER

#endif // _MMEMORY_LEAK_H