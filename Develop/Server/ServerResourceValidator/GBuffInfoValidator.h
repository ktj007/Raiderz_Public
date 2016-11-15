#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"

class GBuffInfo;

class GVALIDATOR_API GBuffInfoValidator : public CSBaseValidator
{
private:
	void CheckTiming(GBuffInfo* pBuff);
	void CheckID(GBuffInfo* pBuff);
	void CheckUseTalent(GBuffInfo* pBuff);	
	void CheckUpkeep(GBuffInfo* pBuff);
	void CheckRequirement(GBuffInfo* pBuff);
	void CheckTrigger(GBuffInfo* pBuff);
	void CheckReleaseCondition(GBuffInfo* pBuff);
	void CheckEnchant(GBuffInfo* pBuff);

public:
	virtual bool Check();
};
