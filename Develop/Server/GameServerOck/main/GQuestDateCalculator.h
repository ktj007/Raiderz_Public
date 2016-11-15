#ifndef _GQUEST_DATE_CALCULATOR_H
#define _GQUEST_DATE_CALCULATOR_H


#include "GTimeType.h"


class GQuestDateCalculator
{
public :
	static void GetAccpetDt(wstring& strAcceptDt);
	static void GetExpiDt(const bool bChallange, wstring& strExpiDt);
	
private :
	static bool GetChallangeQuestExpiDt(wstring& strExpiDt);
	static void GetNormalQuestExpiDt(wstring& strExpiDt);
};


#endif