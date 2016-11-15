#pragma once

#include "TTalentInfo.h"

class TTalentHelper
{
public:
	TTalentHelper() {}

	static TTalentInfo* GetTalentInfo(int nID, int nTalentMode);
	static CSTalentInfo* GetTalentInfo(string strTalentName);

};