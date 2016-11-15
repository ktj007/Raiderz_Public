#include "stdafx.h"

uint32 GSystem::GetNowTime()
{
	return timeGetTime();
}


struct tm* GSystem::GetLocalTime()
{
	__time64_t long_time;
	_time64( &long_time );

	/* Obtain coordinated universal time: */
	return _localtime64( &long_time );
}

void GSystem::Sleep( float fSeconds )
{
	::Sleep((int)(fSeconds*1000.0f));
}