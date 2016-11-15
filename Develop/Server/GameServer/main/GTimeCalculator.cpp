#include "stdafx.h"
#include "GTimeCalculator.h"

bool GTimeCalculator::GetTimeAsString( tm& tmTime, GTimeType& tmOffset )
{
	time_t	timer = time(NULL) + tmOffset.GetSec();

	if (0 != localtime_s(&tmTime, &timer))
		return false;

	tmTime.tm_year += 1900;

	return true;
}

std::wstring GTimeCalculator::GetTimeAsString( GTimeType& tmOffset )
{
	time_t	timer = time(NULL) + tmOffset.GetSec();

	tm tmTime;
	if (0 != localtime_s(&tmTime, &timer))
		return false;

	tmTime.tm_year += 1900;

	CString strRegDate(L"NULL");

	tm RegDate;
	if (GTimeCalculator::GetTimeAsString(RegDate, GTimeSec(0)))
	{
		strRegDate.Format(L"'%d-%d-%d %d:%d:%d'"
			, RegDate.tm_year, RegDate.tm_mon + 1, RegDate.tm_mday, RegDate.tm_hour, RegDate.tm_min, RegDate.tm_sec);
	}

	return wstring(strRegDate.GetBuffer());
}
