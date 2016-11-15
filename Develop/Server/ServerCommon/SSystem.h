#pragma once

#include <time.h>
#include "MTypes.h"
#include "ServerCommonLib.h"


/// ╫ц╫╨еш
class SCOMMON_API SSystem
{
public:
	SSystem() {}
	virtual ~SSystem() {}
	virtual uint32		GetNowTime();
	virtual struct tm*	GetLocalTime();
	virtual void		Sleep(float fSeconds);
};
