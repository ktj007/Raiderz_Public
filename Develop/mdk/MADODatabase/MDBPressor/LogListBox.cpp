#include "StdAfx.h"
#include "LogListBox.h"

//	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console" )

LogListBox::LogListBox( CListBox* pListBox )
{
	m_pLogListBox	= pListBox;
}

LogListBox::~LogListBox()
{
}

void LogListBox::PutLog( const char* szLog, ... )
{
	char szLogString[256];

	va_list arglist; // 함수에서 넘어오는 변수의 목록이 들어간다. 
	va_start(arglist, szLog);

	vsnprintf( szLogString, 256, szLog, arglist );

	va_end(arglist);

	m_pLogListBox->AddString( szLogString );
	m_pLogListBox->SetTopIndex(m_pLogListBox->GetCount()-1);
}

void LogListBox::Dump()
{
	int nLogCount	= m_pLogListBox->GetCount();

	for (int i = 0; i < nLogCount; ++i)
	{
		CString strLogLine;

		m_pLogListBox->GetText(i, strLogLine);

		strLogLine	+= "\r\n";

		printf( strLogLine.GetString() );
		mlog( strLogLine.GetString() );
	}
}
