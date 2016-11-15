#ifndef _GSTRINGS_H
#define _GSTRINGS_H

#include "CSStrings.h"

class GStrings : public CSStrings
{
public:
	static bool			Init();

	static wstring		tmToString(const tm& date);
};




#endif // _GSTRINGS_H