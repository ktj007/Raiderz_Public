#pragma once

#include "GValidatorLib.h"
#include "CSBaseValidator.h"
#include "GChallengerQuestMgr.h"

class GVALIDATOR_API GChallengerQuestInfoValidator : public CSBaseValidator
{
public:
	virtual bool Check() override;

private:
	void CheckTier(int nTier, const GChallengerQuestMgr::Tier* pTier);
};
