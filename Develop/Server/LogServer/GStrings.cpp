#include "StdAfx.h"
#include "GStrings.h"


string GStrings::tmToString(const tm& date)
{
	CString strDate;
	char szYear[4];
	char szMonth[2];
	char szDay[2];
	char szHour[2];
	char szMin[2];
	char szSecond[2];

	_itoa(date.tm_year, szYear, 10);
	_itoa(date.tm_mon, szMonth, 10);
	_itoa(date.tm_mday, szDay, 10);
	_itoa(date.tm_hour, szHour, 10);
	_itoa(date.tm_min, szMin, 10);
	_itoa(date.tm_sec, szSecond, 10);

	strDate.Format(szYear,"-",szMonth,"-",szDay,"-",szHour,"-",szMin,"-",szSecond);

	return LPSTR(LPCTSTR(strDate));
}