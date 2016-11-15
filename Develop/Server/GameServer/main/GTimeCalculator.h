#ifndef _GDATE_CALCULATOR_H
#define _GDATE_CALCULATOR_H

#include "GTimeType.h"

class GTimeCalculator
{
public :
	static bool GetTimeAsString(tm& tmTime, GTimeType& tmOffset);
	static wstring GetTimeAsString(GTimeType& tmOffset);
};


#endif