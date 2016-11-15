#include "StdAfx.h"
#include "PStrings.h"


wstring GStrings::tmToString(const tm& date)
{
	CString strDate;
	wchar_t szYear[4];
	wchar_t szMonth[2];
	wchar_t szDay[2];
	wchar_t szHour[2];
	wchar_t szMin[2];
	wchar_t szSecond[2];

	_itow(date.tm_year, szYear, 10);
	_itow(date.tm_mon, szMonth, 10);
	_itow(date.tm_mday, szDay, 10);
	_itow(date.tm_hour, szHour, 10);
	_itow(date.tm_min, szMin, 10);
	_itow(date.tm_sec, szSecond, 10);

	strDate.Format(szYear,L"-",szMonth,L"-",szDay,L"-",szHour,L"-",szMin,L"-",szSecond);

	return strDate.GetString();
}
