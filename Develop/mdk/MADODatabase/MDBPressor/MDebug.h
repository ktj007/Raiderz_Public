#ifndef __MDEBUG_H
#define __MDEBUG_H

#include <Windows.h>
#include <crtdbg.h>
#include <string>
#include <vector>
#include <set>
using namespace std;

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

	MLOG_FILED			= (MLOG_FILE | MLOG_DEBUG),
	MLOG_PROGD			= (MLOG_PROG | MLOG_DEBUG),
	MLOG_ALL			= (1|2|4|8)	///< 전체
};

/// 콜백 함수
typedef void (MLoggerCustomOutputCallback) (const char* szText, MLogLevel nLevel);	///< Custom 출력

struct MLogSysInfo
{
	string	strName;
	string	strValue;
};

/// 쓰레드에 안전한 로그 시스템(싱글턴)
class MLogger
{
private:
	char							m_szFileName[256];			///< 텍스트 파일 이름
	char							m_szXmlFileName[256];		///< XML 파일 이름
	MLogLevel						m_nLevel;					///< 출력 레벨
	unsigned long					m_nOutputFlags;				///< 출력 방식
	bool							m_bPrintHeader;				///< 헤더를 출력할지 여부
	set<string>						m_Filters;					///< 로그 필터 - 여기에 설정된 필터만 로그를 추가로 남긴다
	MLoggerCustomOutputCallback*	m_fnCustomOutputCallback;	///< 프로그램 자체에서 출력하기 위한 콜백변수
	char							m_szStartDate[128];			///< 로그 시작한 날짜
	char							m_szStartTime[128];			///< 로그 시작한 시간
	char							m_szXslPath[256];			///< Xml의 XSL Path
	MCriticalSection				m_csLock;

	void _OutputFile(const char* szTime, const char* szText, MLogLevel nLevel);
	void _OutputXml(const char* szTime, const char* szText, MLogLevel nLevel);
	void _OutputProg(const char* szText, MLogLevel nLevel);
	void _OutputDebugString(const char* szText, MLogLevel nLevel);
	void InitFile();
	void FinalFile();
	void InitXml();
	void FinalXml();
public:
						MLogger();
						~MLogger();
	void				Init(MLogLevel nLevel=MLOG_LEVEL_DEBUG, 
							 unsigned long int nOutputFlags=MLOG_DEBUG, 
							 const char* szFileName="mlog.txt",
							 const char* szXmlFileName="mlog.xml",
							 const char* szXslPath="" );
	void				AddFilter(const char* szFilter);		// 필터 추가
	void				Output(const char* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);
	void				Output(unsigned long int nOutputFlags, const char* szText, MLogLevel nLevel=MLOG_LEVEL_INFORMATION);
	void				Output(const char* szFilter, const char* szText, MLogLevel nLevel);
	void				OutputSysInfo(vector<MLogSysInfo>& vecSysInfos);	///< 시스템 정보 출력
	void				OnCrash();											///< Crash났을 때 크래쉬 정보를 출력
	void				SetCustomOutputCallback(MLoggerCustomOutputCallback* pCallback) { m_fnCustomOutputCallback=pCallback; }
	void				SetXSLPath(const char* szPath);
	MLogLevel			GetLevel()		{ return m_nLevel; }
	void				SetPrintHeader(bool bPrintHeader)	{ m_bPrintHeader = bPrintHeader; }
};


/// 로그 초기화 
MLogger* MInitLog(MLogLevel nLogLevel=MLOG_LEVEL_DEBUG, 
				unsigned long int nOutputFlags=MLOG_DEBUG,
				MLoggerCustomOutputCallback* pCallback=NULL,
				const char* szFileName="mlog.txt",
				const char* szXmlFileName=NULL,
				const char* szXSLPath=NULL);
/// 로그에 필터 추가
void MAddLogFilter(const char* szFilter);

/// 다음 함수를 호출하면 MLogger의 외부적인 영향이 제거됩니다.
void MFinalizeLog();

MLogger* MGetDefaultLogger();

// 초기화시 설정한 출력방법으로 로그 출력
void __cdecl MLog0(const char* pFormat, ...);		// debug level
void __cdecl MLog1(const char* pFormat, ...);		// information level
void __cdecl MLog2(const char* pFormat, ...);		// warning level
void __cdecl MLog3(const char* pFormat, ...);		// error level

// 출력방법으로 임의로 정하여 로그 출력
void __cdecl MLog0(unsigned long int nOutputFlags, const char* pFormat, ...);		// debug level
void __cdecl MLog1(unsigned long int nOutputFlags, const char* pFormat, ...);		// information level
void __cdecl MLog2(unsigned long int nOutputFlags, const char* pFormat, ...);		// warning level
void __cdecl MLog3(unsigned long int nOutputFlags, const char* pFormat, ...);		// error level

// 필터링될 수 있는 로그 출력
void __cdecl mlog_filter(const char* szFilter, const char* pFormat, ...);

#if defined( _DEBUG ) && defined( _MLOG_DETAIL )

#define mlog	MLog1("%s(%d)", __FILE__, __LINE__); MLog1
#define mlog0	MLog0("%s(%d)", __FILE__, __LINE__); MLog0
#define mlog1	MLog1("%s(%d)", __FILE__, __LINE__); MLog1
#define mlog2	MLog2("%s(%d)", __FILE__, __LINE__); MLog2
#define mlog3	MLog3("%s(%d)", __FILE__, __LINE__); MLog3
#define mlogf	MLog1("%s(%d)", __FILE__, __LINE__); mlog_filter

#else

#define mlog	MLog1			// information level
#define mlog0	MLog0			// debug level
#define mlog1	MLog1			// information level
#define mlog2	MLog2			// warning level
#define mlog3	MLog3			// error level
#define mlogf	mlog_filter

#endif

#endif