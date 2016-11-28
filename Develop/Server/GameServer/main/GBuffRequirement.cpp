#include "StdAfx.h"
#include "GBuffRequirement.h"
#include "GEntityActor.h"
#include "GModuleBuff.h"

bool GBuffRequirement::ProcessRequirement( GEntityActor* pEntity, RequireBuff InfoRequirement )
{
	VALID_RET(pEntity, false);

	if (!InfoRequirement.vecExcludeBuffID.empty())
	{
		for each (int each in InfoRequirement.vecExcludeBuffID)
		{
			if (each < 0)
			{
				if (pEntity->HasBuffLine(-each))
					return false;
			}
			else
			{
				if (pEntity->HasBuff(each))
					return false;
			}
		}

		if (!InfoRequirement.bKeepExcludeBuff)
		{
			LostBuffs(pEntity, InfoRequirement.vecExcludeBuffID);
		}
	}

	if (!InfoRequirement.vecIncludeBuffID.empty())
	{
		for each (int each in InfoRequirement.vecIncludeBuffID)
		{
			if (each < 0)
			{
				if (!pEntity->HasBuffLine(-each))
					return false;
			}
			else
			{
				if (!pEntity->HasBuff(each))
					return false;
			}
		}

		if (!InfoRequirement.bKeepIncludeBuff)
		{
			LostBuffs(pEntity, InfoRequirement.vecIncludeBuffID);
		}
	}
	
	return true;
}

void GBuffRequirement::LostBuffs( GEntityActor* pEntity, const vector<int>& vecBuffs )
{
	VALID(pEntity);
	GModuleBuff* pmBuff = pEntity->GetModuleBuff();
	for each (int each in vecBuffs)
	{
		if (each < 0)
		{
			pmBuff->CancelBuffByLine(-each);
		}
		else
		{
			pmBuff->CancelBuff(each);
		}
	}
}