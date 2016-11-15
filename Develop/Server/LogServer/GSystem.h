#ifndef _GSYSTEM_H
#define _GSYSTEM_H

#include <time.h>

/// ╫ц╫╨еш
class GSystem
{
public:
	virtual uint32		GetNowTime();
	virtual struct tm*	GetLocalTime();
	virtual void		Sleep(float fSeconds);
};



#endif