#ifndef __MDEBUG_H
#define __MDEBUG_H

#include <Windows.h>
#include <crtdbg.h>
#include <string>
#include <vector>
#include <set>
using namespace std;

#include "RLib.h"
#include "MSync.h"

#define MASSERT(x) _ASSERT(x)


#ifdef _WIN32
void __cdecl MMsg(const char *pFormat,...);
#endif

DWORD MFilterException(LPEXCEPTION_POINTERS p);

void MInstallSEH();	// Compile Option에 /EHa 있어야함

///////////////////////////////////////////////////////////////////////////////////////////
// MDebug /////////////
enum MLogLevel
{
	MLOG_LEVEL_DEBUG			= 0,		///< 디버그 정보
	MLOG_LEVEL_INFORMATION		= 1,		///< 정보
	MLOG_LEVEL_WARNING			= 2,		///< 경고
	MLOG_LEVEL_ERROR			= 3,		///< 에러
	MLOG_LEVEL_NONE
};

/// 로그 출력 방법
enum MLogOutput
{
	MLOG_FILE			= 1,		///< 텍스트 파일로 출력
	MLOG_XML			= 2,		///< xml 파일로 출력
	MLOG_DEBUG			= 4,		///< 디버그창으로 출력
	MLOG_PROG			= 8,		///< 프로그램 자체에서 출력
	MLOG_CONSOLE		= 16,		///< 콘솔창(printf)로 출력

	MLOG_FILED			= (MLOG_FILE | MLOG_DEBUG),
	MLOG_PROGD			= (MLOG_PROG | MLOG_DEBUG),
	MLOG_ALL			= (1|2|4|8|16)	///< 전체
};

/// 콜백 함수
typedef void (MLoggerCustomOutputCallback) (const char* szText, MLogLevel nLevel);	///< Custom 출력

struct MLogSysInfo
{
	string	strName;
	string	strValue;
};

/// 쓰레드에 안전한 로그 시스템(싱글턴)
class CML2_API MLogger
{
private:
	char							m_szFileName[256];			///< 텍스트 파일 이름
	char							m_szXmlFileName[256];		///< XML 파일 이름
	MLogLevel						m_nLevel;					///< 출력 레벨
	unsigned long					m_nOutputFlags;				///< 출력 방식
	bool							m_bPrintHeader;				///< 헤더를 출력할지 여부
#pragma warning( push )
#pragma warning( disable : 4251 )
	set<string>						m_Filters;					///< 로그 필터 - 여기에 설정된 필터만 로그를 추가로 남긴다
#pragma warning( pop )

	MLoggerCustomOutputCallback*	m_fnCustomOutputCallback;	///< 프로그램 자체에서 출력하기 위한 콜백변수
	char							m_szStartDate[128];			///< 로그 시작한 날짜
	char							m_szStartTime[128];			///< 로그 시작한 시간
	char							m_szXslPath[256];			///< Xml의 XSL Path
	char							m_szLastError[256];			///< 팝업 메시지 박스 출력 용도의 에러 메시지 보관
	MCriticalSection				m_csLock;

	// Output UTF-8-BOM
	void _OutputFile(const char* szTime, const char* szText, MLogLevel nLevel);	
	void _OutputFile(const WCHAR* szTime, const WCHAR* szText, MLogLevel nLevel );
	void _OutputXml(const char* szTime, const char* szText, MLogLevel nLevel);	
	void _OutputXml(const WCHAR* szTime, const WCHAR* szText, MLogLevel nLevel);

	// Output ANSI
	void _OutputProg(const char* szText, MLogLevel nLevel);
	void _OutputDebugString(const char* szText, MLogLevel nLevel);
	void _OutputConsole(const char* szText, MLogLevel nLevel);
	void _OutputProg(const WCHAR* szText, MLogLevel nLevel);
	void _OutputDebugString(const WCHAR* szText, MLogLevel nLevel);
	void _OutputConsole(const WCHAR* szText, MLogLevel nLevel);

	void InitFile();
	void FinalFile();
	void InitXml();
	void FinalXml();

	FILE* CreateUTF8File(const char* pFileName);

public:
						MLogger();
						~MLogger();
	void				Init(MLogLevel nLevel=MLOG_LEVEL_DEBUG, 
							 unsigned long int nOutputFlags=MLOG_DEBUG, 
							 const char* szFileName="mlog.txt",
							 const char* szXmlFileName="mlog.xml",
							 const char* szXslPath="" );
	void				AddFilter(const char* szFilter);		// 필터 추가
	
	/// ANSI CODE
	void				Output(const char* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);
	void				Output(unsigned long int nOutputFlags, const char* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);
	void				Output(const char* szFilter, const char* szText, MLogLevel nLevel);
	/// UNICODE
	void				Output( const WCHAR* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);
	void				Output( unsigned long int nOutputFlags, const WCHAR* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);


	void				OutputSysInfo(vector<MLogSysInfo>& vecSysInfos);	///< 시스템 정보 출력
	void				OnCrash();											///< Crash났을 때 크래쉬 정보를 출력
	void				SetCustomOutputCallback(MLoggerCustomOutputCallback* pCallback) { m_fnCustomOutputCallback=pCallback; }
	void				SetXSLPath(const char* szPath);
	MLogLevel			GetLevel()		{ return m_nLevel; }
	void				SetPrintHeader(bool bPrintHeader)	{ m_bPrintHeader = bPrintHeader; }
	void				SetLastError(const char* szText);
	const char*			GetLastError();
	const char*			GetFileName()	{ return m_szFileName; }
};
extern CML2_API MLogger g_DefaultLogger;

/// 로그 초기화 
CML2_API MLogger* MInitLog(	MLogLevel nLogLevel=MLOG_LEVEL_DEBUG, 
							unsigned long int nOutputFlags=MLOG_DEBUG,
							MLoggerCustomOutputCallback* pCallback=NULL,
							const char* szFileName="mlog.txt",
							const char* szXmlFileName=NULL,
							const char* szXSLPath=NULL);
/// 로그에 필터 추가
CML2_API void MAddLogFilter(const char* szFilter);

/// 다음 함수를 호출하면 MLogger의 외부적인 영향이 제거됩니다.
CML2_API void MFinalizeLog();

CML2_API MLogger* MGetDefaultLogger();

// 초기화시 설정한 출력방법으로 로그 출력
CML2_API void __cdecl MLog0(const char* pFormat, ...);		// debug level
CML2_API void __cdecl MLog1(const char* pFormat, ...);		// information level
CML2_API void __cdecl MLog2(const char* pFormat, ...);		// warning level
CML2_API void __cdecl MLog3(const char* pFormat, ...);		// error level
CML2_API void __cdecl MLog0( const WCHAR* pFormat, ... );		// debug level
CML2_API void __cdecl MLog1( const WCHAR* pFormat, ... );		// information level
CML2_API void __cdecl MLog2( const WCHAR* pFormat, ... );		// warning level
CML2_API void __cdecl MLog3( const WCHAR* pFormat, ... );		// error level

// 출력방법으로 임의로 정하여 로그 출력
CML2_API void __cdecl MLog0(unsigned long int nOutputFlags, const char* pFormat, ...);		// debug level
CML2_API void __cdecl MLog1(unsigned long int nOutputFlags, const char* pFormat, ...);		// information level
CML2_API void __cdecl MLog2(unsigned long int nOutputFlags, const char* pFormat, ...);		// warning level
CML2_API void __cdecl MLog3(unsigned long int nOutputFlags, const char* pFormat, ...);		// error level

// 필터링될 수 있는 로그 출력
CML2_API void __cdecl mlog_filter(const char* szFilter, const char* pFormat, ...);

/// 툴에서만 보일 로그
CML2_API void __cdecl mlog_tool(const char* pFormat, ...);

/// LastError
CML2_API void __cdecl mlog_setlasterror(const char* pFormat, ...);
CML2_API void __cdecl mlog_popuplasterror(const char* szTitle = NULL);

#if defined( _DEBUG ) && defined( _MLOG_DETAIL )

#define mlog	MLog1("%s(%d)", __FILE__, __LINE__); MLog1
#define mlog0	MLog0("%s(%d)", __FILE__, __LINE__); MLog0
#define dlog	MLog0("%s(%d)", __FILE__, __LINE__); MLog0
#define mlog1	MLog1("%s(%d)", __FILE__, __LINE__); MLog1
#define mlog2	MLog2("%s(%d)", __FILE__, __LINE__); MLog2
#define mlog3	MLog3("%s(%d)", __FILE__, __LINE__); MLog3
#define mlogf	MLog1("%s(%d)", __FILE__, __LINE__); mlog_filter

#else

#define mlog	MLog1			// information level
#define mlog0	MLog0			// debug level
#define dlog	MLog0			// debug level
#define mlog1	MLog1			// information level
#define mlog2	MLog2			// warning level
#define mlog3	MLog3			// error level
#define mlogf	mlog_filter

#endif



/// MPrivateLogger : 클래스마다 따로 로그를 남기고 싶을때 각자 설정해서 사용할수 있도록 멤버 함수로 만들어주는 헬퍼
/// 상속을 받고, mclog0~3 을 사용하여 로그를 남기면 된다, 초기화 ( SetLogger ) 는 필요하다
class CML2_API MPrivateLogger
{
public:
	MPrivateLogger() : m_pLogger(NULL) {}
	void SetLogger( MLogger* pLogger ) { m_pLogger = pLogger; }
	void Log(MLogLevel nLevel, const char* pFormat, ...);

protected:
	MLogger*	m_pLogger;
};

#define mplog0(...)	Log(MLOG_LEVEL_DEBUG		, __VA_ARGS__)
#define mplog1(...)	Log(MLOG_LEVEL_INFORMATION	, __VA_ARGS__)
#define mplog2(...)	Log(MLOG_LEVEL_WARNING		, __VA_ARGS__)
#define mplog3(...)	Log(MLOG_LEVEL_ERROR		, __VA_ARGS__)
#define mplog		mplog1

#endif