#include "stdafx.h"
#include "SLogger.h"
#include "FileInfo.h"
#include "SAppHelper.h"
#include "MLocale.h"

void SLogger::AddPathSeperator( wstring& strOut )
{
	int len = (int)strOut.size();

	if (len > 0)
	{
		if (strOut[len-1] != L'\\' &&
			strOut[len-1] != L'/')
		{
			strOut = strOut + L"/";
		}
	}
}

void SLogger::Init( MLogLevel nLogLevel, wstring strLogPath)
{
	if (strLogPath.size() > 0)
	{
		AddPathSeperator(strLogPath);

		wchar_t szPath[_MAX_PATH];
		wcscpy_s(szPath, strLogPath.c_str());

		CreatePath( szPath );
	}

	wstring strFileName = MakeFileName(strLogPath);

	unsigned long int nOutputFlags = MLOG_DEBUG | MLOG_PROG | MLOG_FILE;

	MInitLog(nLogLevel, nOutputFlags, NULL, MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str());
}

wstring SLogger::MakeFileName( wstring strLogPath )
{
	wstring strFileName = strLogPath;

	strFileName += LOG_FILE_HEADER;

#ifndef _DEBUG
	strFileName += SAppHelper::MakeCurrentDateTimeStringForFileName();
#endif

	strFileName += L".txt";

	return strFileName;
}