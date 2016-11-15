#include "stdafx.h"
#include "SSystem.h"
#include "mmsystem.h"

uint32 SSystem::GetNowTime()
{
	return timeGetTime();
}

struct tm* SSystem::GetLocalTime()
{
	__time64_t long_time;
	_time64( &long_time );

	/* Obtain coordinated universal time: */
	return _localtime64( &long_time );
}

void SSystem::Sleep( float fSeconds )
{
	::Sleep((int)(fSeconds*1000.0f));
}
