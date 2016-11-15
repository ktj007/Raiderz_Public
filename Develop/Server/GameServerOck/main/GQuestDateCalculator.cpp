#include "stdafx.h"
#include "GQuestDateCalculator.h"
#include "GTimeCalculator.h"


void GQuestDateCalculator::GetExpiDt( const bool bChallange, wstring& strExpiDt )
{
	if (bChallange)
	{
		if (GetChallangeQuestExpiDt(strExpiDt))
			return;
	}

	GetNormalQuestExpiDt(strExpiDt);
}

bool GQuestDateCalculator::GetChallangeQuestExpiDt( wstring& strExpiDt )
{
	tm cur_time;

	if (!GTimeCalculator::GetTimeAsString(cur_time, GTimeDay(0)))
		return false;

	CString strDate;

	if (4 <= cur_time.tm_hour)
	{
		tm next_time;

		if (!GTimeCalculator::GetTimeAsString(next_time, GTimeDay(1)))
			return false;		

		strDate.Format(L"%d-%d-%d 4:30:0", next_time.tm_year, next_time.tm_mon + 1, next_time.tm_mday);		
	}
	else
	{
		strDate.Format(L"%d-%d-%d 4:30:0", cur_time.tm_year, cur_time.tm_mon + 1, cur_time.tm_mday);
	}

	strExpiDt = strDate.GetBuffer();

	return true;
}

void GQuestDateCalculator::GetNormalQuestExpiDt( wstring& strExpiDt )
{
	strExpiDt = L"9999-12-31 04:30";
}

void GQuestDateCalculator::GetAccpetDt(wstring& strAcceptDt )
{
	tm cur_time;

	if (!GTimeCalculator::GetTimeAsString(cur_time, GTimeDay(0)))
	{
		strAcceptDt = L"9999-12-31 00:00:00";
		return;
	}

	CString strDate;

	strDate.Format(L"%d-%d-%d %d:%d:%d"
		, cur_time.tm_year, cur_time.tm_mon + 1, cur_time.tm_mday
		, cur_time.tm_hour, cur_time.tm_min, cur_time.tm_sec);

	strAcceptDt = strDate.GetBuffer();
}
