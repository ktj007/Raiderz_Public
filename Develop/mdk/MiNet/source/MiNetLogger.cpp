#include "stdafx.h"
#include "MiNetLogger.h"
#include "MDebug.h"

namespace minet {

void MiNetLogger::Log( const char* szText, ... )
{
	const int BUF_LEN = 8192;

	char temp[BUF_LEN];

	va_list args;
	va_start(args, szText);
	vsprintf_s(temp, szText, args);
	va_end(args);

	mlog(temp);
}

void MiNetLogger::DebugLog( const char* szText, ... )
{
#ifndef _DEBUG
	return;
#endif

	const int BUF_LEN = 8192;

	char temp[BUF_LEN];

	va_list args;
	va_start(args, szText);
	vsprintf_s(temp, szText, args);
	va_end(args);

	mlog(temp);
}

} // namespace minet {