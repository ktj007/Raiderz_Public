#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include "fileinfo.h"
#include "MDebug.h"
#include "MPdb.h"
#include <windows.h>
#include <iostream>
#include "MLocale.h"

#ifdef _WIN32
#include <windows.h>
#include <crtdbg.h>


void __cdecl MMsg(const char *pFormat,...)
{
    char buff[256];

    wvsprintf(buff, pFormat, (char *)(&pFormat+1));
    lstrcat(buff, "\r\n");
    MessageBox( NULL, buff, "RealSpace Message", MB_OK );
	mlog(buff);mlog("\n");
}
#endif

////////////////////////////////////////////////////////////////////////////////
// exception handler

void MShowContextRecord(CONTEXT* p)
{
	mlog("[Context]\n");

	mlog("GS : %08x  FS : %08x  ES : %08x  DS : %08x\n", p->SegGs, p->SegFs, p->SegEs, p->SegDs);
	mlog("EDI: %08x  ESI: %08x  EBX: %08x  EDX: %08x\n", p->Edi, p->Esi, p->Ebx, p->Edx);
	mlog("ECX: %08x  EAX: %08x  EBP: %08x  EIP: %08x\n", p->Ecx, p->Eax, p->Ebp, p->Eip);
	mlog("CS : %08x  Flg: %08x  ESP: %08x  SS : %08x\n", p->SegCs, p->EFlags, p->Esp, p->SegSs);

	mlog("\n");
}

void MShowStack(DWORD* sp, int nSize)
{
	mlog("[Stack]");

	for(int i=0; i<nSize; i++){
		if((i%8)==0) mlog("\n");
		mlog("%08x ", *(sp+i));
	}

	mlog("\n");
}

DWORD MFilterException(LPEXCEPTION_POINTERS p)
{
	char tmpbuf[128];
	_strtime( tmpbuf );
	mlog("Crash ( %s )\n",tmpbuf);

	mlog("Build "__DATE__" "__TIME__"\n\n");

	mlog("\n[Exception]\n");
	mlog("Address	:	%08x\n", p->ExceptionRecord->ExceptionAddress);
	mlog("ExpCode	:	%08x\n", p->ExceptionRecord->ExceptionCode);
	mlog("Flags	:	%08x\n", p->ExceptionRecord->ExceptionFlags);
	mlog("#Param	:	%08x\n", p->ExceptionRecord->NumberParameters);
	mlog("other	:	%08x\n", p->ExceptionRecord->ExceptionRecord);
	mlog("\n");
	
	MShowContextRecord(p->ContextRecord);
	MShowStack((DWORD*)p->ContextRecord->Esp, 128);

	mlog("\n");

	string str;

	GetCrashInfo(p,str);

	mlog(str.c_str());
	mlog("\n");

	return EXCEPTION_EXECUTE_HANDLER;
}

void MSEHTranslator(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{
	MFilterException(pExcPointers);

	raise(SIGABRT);     // raise abort signal 

	// We usually won't get here, but it's possible that
	// SIGABRT was ignored.  So hose the program anyway.
	_exit(3);
}

void MInstallSEH()	// Compile Option에 /EHa 있어야함
{
	#ifndef _DEBUG
		_set_se_translator(MSEHTranslator);	 // 이 함수는 쓰레드 단위에서만 작동한다.
	#endif
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// MLogger /////////////////////////////////////////////////////////////////////////////////////////
MLogger g_DefaultLogger;

MLogger* MInitLog(MLogLevel nLogLevel, 
				unsigned long int nOutputFlags, 
				MLoggerCustomOutputCallback* pCallback,
				const char* szFileName, 
				const char* szXmlFileName, 
				const char* szXSLPath)
{
	g_DefaultLogger.Init(nLogLevel, nOutputFlags, szFileName, szXmlFileName, szXSLPath);
	g_DefaultLogger.SetPrintHeader(true);
	g_DefaultLogger.SetCustomOutputCallback(pCallback);
	return &g_DefaultLogger;
}

void MFinalizeLog()
{
	g_DefaultLogger.SetCustomOutputCallback(NULL);
}

void MAddLogFilter(const char* szFilter)
{
	g_DefaultLogger.AddFilter(szFilter);
}

MLogger* MGetDefaultLogger()
{
	return &g_DefaultLogger;
}

#define MLOG_TEMP_BUF_LEN		8192		// 8k

void __cdecl MLog0(const char *pFormat, ...)
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_DEBUG < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_DEBUG);
}
void __cdecl MLog1(const char *pFormat, ...)
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_INFORMATION < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_INFORMATION);
}
void __cdecl MLog2(const char *pFormat, ...)
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_WARNING < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_WARNING);
}
void __cdecl MLog3(const char *pFormat, ...)
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_ERROR);
}
void __cdecl MLog0(unsigned long int nOutputFlags, const char *pFormat, ...)
{
	if (MLOG_LEVEL_DEBUG < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(nOutputFlags, temp, MLOG_LEVEL_DEBUG);

}
void __cdecl MLog1(unsigned long int nOutputFlags, const char *pFormat, ...)
{
	if (MLOG_LEVEL_INFORMATION < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(nOutputFlags, temp, MLOG_LEVEL_INFORMATION);

}
void __cdecl MLog2(unsigned long int nOutputFlags, const char *pFormat, ...)
{
	if (MLOG_LEVEL_WARNING < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(nOutputFlags, temp, MLOG_LEVEL_WARNING);
}
void __cdecl MLog3(unsigned long int nOutputFlags, const char *pFormat, ...)
{
	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(nOutputFlags, temp, MLOG_LEVEL_ERROR);
}

/// UNICODE <<<
void __cdecl MLog0( const WCHAR *pFormat, ... )
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_DEBUG < g_DefaultLogger.GetLevel()) return;

	WCHAR temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s( temp, MLOG_TEMP_BUF_LEN, pFormat, args );
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_DEBUG);
}
void __cdecl MLog1( const WCHAR *pFormat, ... )
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_INFORMATION < g_DefaultLogger.GetLevel()) return;

	WCHAR temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s( temp, MLOG_TEMP_BUF_LEN, pFormat, args );
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_INFORMATION);
}
void __cdecl MLog2( const WCHAR *pFormat, ... )
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	if (MLOG_LEVEL_WARNING < g_DefaultLogger.GetLevel()) return;

	WCHAR temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s( temp, MLOG_TEMP_BUF_LEN, pFormat, args );
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_WARNING);
}
void __cdecl MLog3( const WCHAR *pFormat, ... )
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 
	WCHAR temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vswprintf_s( temp, MLOG_TEMP_BUF_LEN, pFormat, args );
	va_end(args);

	g_DefaultLogger.Output(temp, MLOG_LEVEL_ERROR);
}
/// UNICODE >>>

void __cdecl mlog_filter(const char* szFilter, const char* pFormat, ...)
{
	if (MLOG_LEVEL_DEBUG < g_DefaultLogger.GetLevel()) return;

	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output(szFilter, temp, MLOG_LEVEL_DEBUG);
}

void __cdecl mlog_tool(const char* pFormat, ...)
{
	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.Output("tool", temp, MLOG_LEVEL_INFORMATION);
}

void __cdecl mlog_setlasterror(const char* pFormat, ...)
{
	char temp[MLOG_TEMP_BUF_LEN];

	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	g_DefaultLogger.SetLastError(temp);
	mlog3(temp);
}

void __cdecl mlog_popuplasterror( const char* szTitle )
{
	const char* szText = g_DefaultLogger.GetLastError();
	if( szText )
		MessageBox( NULL, szText, szTitle, MB_OK );
}

/////////////////////////////////////////////////////////////////////////
// MLogger //////////////////////////////////////////////////////////////
MLogger::MLogger()
{
	m_szFileName[0] = 0;
	m_szXmlFileName[0] = 0;
	m_szStartDate[0] = 0;
	m_szStartTime[0] = 0;
	m_szXslPath[0] = 0;
	m_szLastError[0] = 0;
	m_nLevel = MLOG_LEVEL_ERROR;
	m_nOutputFlags = MLOG_DEBUG;
	m_fnCustomOutputCallback = NULL;
	m_bPrintHeader = false;
}

MLogger::~MLogger()
{
	if ((m_nOutputFlags & MLOG_XML) && (m_szXmlFileName != NULL))
	{
		FinalXml();
	}
}

void MLogger::Init(MLogLevel nLevel, unsigned long int nOutputFlags, const char* szFileName, const char* szXmlFileName, const char* szXslPath)
{
	//MCriticalSection::Guard guard(m_csLock);
	m_csLock.Lock();

	_strdate( m_szStartDate );
	_strtime( m_szStartTime );

	if ((nOutputFlags & MLOG_FILE) && (szFileName != NULL))
	{
		if(szXslPath != NULL)	wsprintf(m_szFileName, "%s%s", szXslPath, szFileName);
		else					GetFullPath(m_szFileName, szFileName);

		InitFile();
	}
	if ((nOutputFlags & MLOG_XML) && (szXmlFileName != NULL) && (szXslPath != NULL))
	{
		GetFullPath(m_szXmlFileName, szXmlFileName);
		strcpy_s(m_szXslPath, szXslPath);
		InitXml();
	}

	m_nLevel = nLevel;
	m_nOutputFlags = nOutputFlags;

	m_csLock.Unlock();
}

void MLogger::AddFilter(const char* szFilter)
{
	//MCriticalSection::Guard guard(m_csLock);
	m_csLock.Lock();
	m_Filters.insert(string(szFilter));
	m_csLock.Unlock();
}

void MLogger::InitFile()
{
	FILE* pFile = CreateUTF8File(m_szFileName);
	if (pFile == NULL) return;

	fprintf(pFile, "------------------------------------------------------------------------------\n");
	fprintf(pFile, "-- Logging Start (%s %s)\n", m_szStartDate, m_szStartTime);
	fprintf(pFile, "------------------------------------------------------------------------------\n");

	fclose(pFile);
}

void MLogger::InitXml()
{
	FILE* pFile = CreateUTF8File(m_szXmlFileName);
	if (pFile == NULL) return;

	fprintf(pFile, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");

	if (strlen(m_szXslPath) > 0)
	{
		fprintf(pFile, "<?xml-stylesheet type=\"text/xsl\" href=\"%s\"?>\n", m_szXslPath);
	}
	fprintf(pFile, "<log date=\"%s\" time=\"%s\">\n", m_szStartDate, m_szStartTime);

	fclose(pFile);
}

void MLogger::FinalXml()
{
	FILE *pFile;
	pFile=fopen(m_szXmlFileName, "a");
	if( pFile ) 
	{
		fprintf(pFile, "</log>");
		fclose(pFile);
	}
}

FILE* MLogger::CreateUTF8File(const char* pFileName)
{
	FILE *pFile = fopen(pFileName, "w");

	if (pFile)
	{
		static unsigned char UTF8_BOM[3] = { 0xEF, 0xBB, 0xBF};

		fwrite(UTF8_BOM, sizeof(UTF8_BOM), 1, pFile);
	}

	return pFile;
}

void MLogger::OutputSysInfo(vector<MLogSysInfo>& vecSysInfos)
{
	// 시스템 정보는 File로만 출력한다.
	if (m_nOutputFlags & MLOG_FILE)
	{
		FILE *pFile;
		pFile=fopen(m_szFileName, "a");
		if( !pFile ) pFile = CreateUTF8File(m_szFileName);
		
		if(pFile)
		{
			int nItemCount = (int)vecSysInfos.size();
			for (int i = 0; i < nItemCount; i++)
			{
				fprintf(pFile, " - %s : %s\n", MLocale::ConvAnsiToUTF8(vecSysInfos[i].strName.c_str()).c_str(), MLocale::ConvAnsiToUTF8(vecSysInfos[i].strValue.c_str()).c_str());
			}
			fprintf(pFile, "==============================================================================\n");
			fclose(pFile);
		}
	}
	if (m_nOutputFlags & MLOG_XML)
	{
		FILE* pFile;
		pFile = fopen( m_szXmlFileName, "a");
		if( pFile!=NULL )
		{
			fprintf(pFile, "\t<header>\n");

			int nItemCount = (int)vecSysInfos.size();
			for (int i = 0; i < nItemCount; i++)
			{
				fprintf(pFile, "\t\t<sysinfo name=\"%s\">%s</sysinfo>\n", MLocale::ConvAnsiToUTF8(vecSysInfos[i].strName.c_str()).c_str(), MLocale::ConvAnsiToUTF8(vecSysInfos[i].strValue.c_str()).c_str());
			}
			fprintf(pFile, "\t</header>\n");

			fclose(pFile);
		}
	}
}

void MLogger::Output(const char* szText, MLogLevel nLevel)
{
	Output(m_nOutputFlags, szText, nLevel);
}

void MLogger::Output(const char* szFilter, const char* szText, MLogLevel nLevel)
{
	if (m_Filters.find(string(szFilter)) == m_Filters.end()) return;

	Output(m_nOutputFlags, szText, nLevel);
}

void MLogger::Output(unsigned long int nOutputFlags, const char* szText, MLogLevel nLevel)
{
	/// 2009. 11. 12 최흥배
	/// 파일에 로그를 남기는 경우 로그를 쓸 때마다 파일을 열고, 쓰고, 닫기 때문에 이것 자체가 스레드에 
	/// 안전하므로 lock을 걸지 않습니다.

	char szTime[64]="";
	_strtime( szTime );

	if (nOutputFlags & MLOG_FILE)
	{
		_OutputFile(szTime, szText, nLevel);
	}
	if (nOutputFlags & MLOG_XML)
	{
		m_csLock.Lock();
		_OutputXml(szTime, szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_DEBUG)
	{
		m_csLock.Lock();
		_OutputDebugString(szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_PROG)
	{
		m_csLock.Lock();
		_OutputProg(szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_CONSOLE)
	{
		m_csLock.Lock();
		_OutputConsole(szText, nLevel);
		m_csLock.Unlock();
	}
}

/// UNICODE <<<
void MLogger::Output( const WCHAR* szText, MLogLevel nLevel )
{
	Output( m_nOutputFlags, szText, nLevel );
}

void MLogger::Output( unsigned long int nOutputFlags, const WCHAR* szText, MLogLevel nLevel )
{

	WCHAR szTime[64] = {0,};
	_wstrtime( szTime );

	if (nOutputFlags & MLOG_FILE)
	{
		_OutputFile(szTime, szText, nLevel);
	}
	if (nOutputFlags & MLOG_XML)
	{
		m_csLock.Lock();
		_OutputXml(szTime, szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_DEBUG)
	{
		m_csLock.Lock();
		_OutputDebugString(szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_PROG)
	{
		m_csLock.Lock();
		_OutputProg(szText, nLevel);
		m_csLock.Unlock();
	}
	if (nOutputFlags & MLOG_CONSOLE)
	{
		m_csLock.Lock();
		_OutputConsole(szText, nLevel);
		m_csLock.Unlock();
	}
}
/// UNICODE >>>

void MLogger::_OutputFile(const char* szTime, const char* szText, MLogLevel nLevel)
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 

	FILE* pFile;
	pFile = fopen( m_szFileName, "a");
	if( !pFile ) pFile = CreateUTF8File(m_szFileName);

	if( pFile==NULL ) return;

	
	if (m_bPrintHeader)
	{
		switch (nLevel)
		{
		case MLOG_LEVEL_DEBUG:			fprintf_s(pFile, "[d] %s - %s", szTime, MLocale::ConvAnsiToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_INFORMATION:	fprintf_s(pFile, "[i] %s - %s", szTime, MLocale::ConvAnsiToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_WARNING:		fprintf_s(pFile, "[w] %s - %s", szTime, MLocale::ConvAnsiToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_ERROR:			fprintf_s(pFile, "[e] %s - %s", szTime, MLocale::ConvAnsiToUTF8(szText).c_str()); break;
		}
	}
	else
	{
		fprintf_s(pFile, "%s - %s", szTime, MLocale::ConvAnsiToUTF8(szText).c_str());
	}

	fflush(pFile);
	fclose(pFile);
}

void MLogger::_OutputFile( const WCHAR* szTime, const WCHAR* szText, MLogLevel nLevel )
{
	/// <중요> 변경할 때 ANSI 버전과 UNICODE 버전을 같이 작업해야한다. 

	FILE* pFile;
	pFile = fopen( m_szFileName, "a");
	if( !pFile ) pFile = CreateUTF8File(m_szFileName);
	if( pFile==NULL ) return;

	if (m_bPrintHeader)
	{
		switch (nLevel)
		{
		case MLOG_LEVEL_DEBUG:			fprintf_s(pFile, "[d] %s - %s", MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_INFORMATION:	fprintf_s(pFile, "[i] %s - %s", MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_WARNING:		fprintf_s(pFile, "[w] %s - %s", MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str()); break;
		case MLOG_LEVEL_ERROR:			fprintf_s(pFile, "[e] %s - %s", MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str()); break;
		}
	}
	else
	{
		fprintf_s(pFile, "%s - %s", MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str());
	}

	fflush(pFile);
	fclose(pFile);
}

void MLogger::_OutputXml(const char* szTime, const char* szText, MLogLevel nLevel)
{
	FILE* pFile;
	pFile = fopen( m_szXmlFileName, "a");
	if( !pFile ) pFile = CreateUTF8File(m_szXmlFileName);
	if( pFile==NULL ) return;

	char szAttrType[32] = "";
	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			strcpy_s(szAttrType, "debug"); break;
	case MLOG_LEVEL_INFORMATION:	strcpy_s(szAttrType, "information"); break;
	case MLOG_LEVEL_WARNING:		strcpy_s(szAttrType, "warning"); break;
	case MLOG_LEVEL_ERROR:			strcpy_s(szAttrType, "error"); break;
	}

	fprintf(pFile, "<event type=\"%s\" time=\"%s\">%s</event>\n", szAttrType, szTime, MLocale::ConvAnsiToUTF8(szText).c_str());
	fclose(pFile);
}

void MLogger::_OutputXml(const WCHAR* szTime, const WCHAR* szText, MLogLevel nLevel)
{
	FILE* pFile;
	pFile = fopen( m_szXmlFileName, "a");
	if( !pFile ) pFile = CreateUTF8File(m_szXmlFileName);
	if( pFile==NULL ) return;

	char szAttrType[32] = "";
	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			strcpy_s(szAttrType, "debug"); break;
	case MLOG_LEVEL_INFORMATION:	strcpy_s(szAttrType, "information"); break;
	case MLOG_LEVEL_WARNING:		strcpy_s(szAttrType, "warning"); break;
	case MLOG_LEVEL_ERROR:			strcpy_s(szAttrType, "error"); break;
	}

	fprintf(pFile, "<event type=\"%s\" time=\"%s\">%s</event>\n", szAttrType, MLocale::ConvUTF16ToUTF8(szTime).c_str(), MLocale::ConvUTF16ToUTF8(szText).c_str());
	fclose(pFile);
}

void MLogger::_OutputProg(const char* szText, MLogLevel nLevel)
{
	if (m_fnCustomOutputCallback) m_fnCustomOutputCallback(szText, nLevel);
}

void MLogger::_OutputDebugString(const char* szText, MLogLevel nLevel)
{
	OutputDebugStringA(szText);
}

void MLogger::_OutputConsole(const char* szText, MLogLevel nLevel)
{
	DWORD writtenCount;
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), szText, strlen(szText), &writtenCount, NULL);	
}

void MLogger::_OutputProg(const WCHAR* szText, MLogLevel nLevel)
{
	if (m_fnCustomOutputCallback) m_fnCustomOutputCallback(MLocale::ConvUTF16ToAnsi(szText).c_str(), nLevel);
}

void MLogger::_OutputDebugString(const WCHAR* szText, MLogLevel nLevel)
{
	OutputDebugStringW(szText);
}

void MLogger::_OutputConsole(const WCHAR* szText, MLogLevel nLevel)
{
	DWORD writtenCount;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), szText, wcslen(szText), &writtenCount, NULL);
}

void MLogger::OnCrash()
{
	// xml마무리는 따로 해줘야 한다.
	if ((m_nOutputFlags & MLOG_XML) && (m_szXmlFileName != NULL))
	{
		FinalXml();
	}
}

void MLogger::SetLastError(const char* szText)
{
	strncpy_s( m_szLastError, _countof(m_szLastError), szText, _countof(m_szLastError)-1 );
}

const char* MLogger::GetLastError()
{
	if (strlen(m_szLastError))
		return m_szLastError;

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// MSysInfo ////////////////////////////////////////////////////////////////////////////////////////


#define PRIVATE_LOG_TEMP_BUF_LEN		8192		// 8k

 void MPrivateLogger::Log( MLogLevel nLevel, const char* pFormat, ... )
 {
	 char szBuffer[PRIVATE_LOG_TEMP_BUF_LEN];

	 va_list args;
	 va_start(args,pFormat);
	 vsprintf_s(szBuffer,pFormat,args);
	 va_end(args);

	 // 없으면 전역 로거를 쓸까,,
	 if(m_pLogger)
		 m_pLogger->Output( szBuffer, nLevel );
 }
