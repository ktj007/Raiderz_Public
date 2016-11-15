#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include "fileinfo.h"
#include "MDebug.h"
#include "MPdb.h"
#include <windows.h>



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


/////////////////////////////////////////////////////////////////////////
// MLogger //////////////////////////////////////////////////////////////
MLogger::MLogger()
{
	m_szFileName[0] = 0;
	m_szXmlFileName[0] = 0;
	m_szStartDate[0] = 0;
	m_szStartTime[0] = 0;
	m_szXslPath[0] = 0;
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
		GetFullPath(m_szFileName, szFileName);

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
	FILE *pFile;
	pFile=fopen(m_szFileName, "w+");
	if( pFile == NULL) return;

	//fprintf(pFile, "------------------------------------------------------------------------------\n");
	fprintf(pFile, "%s\n", m_szStartTime);
	//fprintf(pFile, "------------------------------------------------------------------------------\n");
	fclose(pFile);
}

void MLogger::InitXml()
{
	FILE *pFile;
	pFile=fopen(m_szXmlFileName, "w+");
	if( pFile == NULL) return;

	fprintf(pFile, "<?xml version=\"1.0\" encoding=\"euc-kr\" ?>\n");

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

void MLogger::OutputSysInfo(vector<MLogSysInfo>& vecSysInfos)
{
	// 시스템 정보는 File로만 출력한다.
	if (m_nOutputFlags & MLOG_FILE)
	{
		FILE *pFile;
		pFile=fopen(m_szFileName, "a");
		if( !pFile ) pFile=fopen(m_szFileName, "w");
		if(pFile)
		{
			int nItemCount = (int)vecSysInfos.size();
			for (int i = 0; i < nItemCount; i++)
			{
				fprintf(pFile, " - %s : %s\n", vecSysInfos[i].strName.c_str(), vecSysInfos[i].strValue.c_str());
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
				fprintf(pFile, "\t\t<sysinfo name=\"%s\">%s</sysinfo>\n", vecSysInfos[i].strName.c_str(), vecSysInfos[i].strValue.c_str() );
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
	//by pok, test
	//int a = m_csLock.m_nCount;
	//MCriticalSection::Guard guard(m_csLock);
//	m_csLock.Lock();

	char szTime[64]="";
	//_strtime( szTime );

	if (nOutputFlags & MLOG_FILE)
	{
		_OutputFile(szTime, szText, nLevel);
	}
	if (nOutputFlags & MLOG_XML)
	{
		_OutputXml(szTime, szText, nLevel);
	}
	if (nOutputFlags & MLOG_DEBUG)
	{
		_OutputDebugString(szText, nLevel);
	}
	if (nOutputFlags & MLOG_PROG)
	{
		_OutputProg(szText, nLevel);
	}

//	m_csLock.Unlock();
}


void MLogger::_OutputFile(const char* szTime, const char* szText, MLogLevel nLevel)
{
	FILE* pFile;
	pFile = fopen( m_szFileName, "a");
	if( !pFile ) pFile=fopen(m_szFileName, "w");
	if( pFile==NULL ) return;

	//if (m_bPrintHeader)
	//{
	//	switch (nLevel)
	//	{
	//	case MLOG_LEVEL_DEBUG:			fprintf(pFile, "[d] %s - ", szTime); break;
	//	case MLOG_LEVEL_INFORMATION:	fprintf(pFile, "[i] %s - ", szTime); break;
	//	case MLOG_LEVEL_WARNING:		fprintf(pFile, "[w] %s - ", szTime); break;
	//	case MLOG_LEVEL_ERROR:			fprintf(pFile, "[e] %s - ", szTime); break;
	//	}
	//}

	fprintf(pFile, "%s", szText);

	fflush(pFile);
	fclose(pFile);
}

void MLogger::_OutputXml(const char* szTime, const char* szText, MLogLevel nLevel)
{
	FILE* pFile;
	pFile = fopen( m_szXmlFileName, "a");
	if( !pFile ) pFile=fopen(m_szXmlFileName, "w");
	if( pFile==NULL ) return;

	char szAttrType[32] = "";
	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			strcpy_s(szAttrType, "debug"); break;
	case MLOG_LEVEL_INFORMATION:	strcpy_s(szAttrType, "information"); break;
	case MLOG_LEVEL_WARNING:		strcpy_s(szAttrType, "warning"); break;
	case MLOG_LEVEL_ERROR:			strcpy_s(szAttrType, "error"); break;
	}

	fprintf(pFile, "<event type=\"%s\" time=\"%s\">%s</event>\n", szAttrType, szTime, szText);
	fclose(pFile);
}

void MLogger::_OutputProg(const char* szText, MLogLevel nLevel)
{
	if (m_fnCustomOutputCallback) m_fnCustomOutputCallback(szText, nLevel);
}

void MLogger::_OutputDebugString(const char* szText, MLogLevel nLevel)
{
	OutputDebugString(szText);
}

void MLogger::OnCrash()
{
	// xml마무리는 따로 해줘야 한다.
	if ((m_nOutputFlags & MLOG_XML) && (m_szXmlFileName != NULL))
	{
		FinalXml();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// MSysInfo ////////////////////////////////////////////////////////////////////////////////////////
