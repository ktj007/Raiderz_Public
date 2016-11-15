#include "stdafx.h"
#include "GGlueQuest.h"
#include "GPlayerQuest.h"
#include "GQuestInfo.h"
#include "GScriptMacro.h"
#include "GQuestSystem.h"
#include "GGlobal.h"
#include "MLocale.h"
#include "GQuestVarUpdater.h"

GGlueQuest::GGlueQuest(GPlayerQuest* pOwner)
{
	m_pOwner = pOwner;
}

GGlueQuest::~GGlueQuest()
{
	// do nothing
}

GPlayerQuest* GGlueQuest::GetOwnerQuest() const
{
	return m_pOwner;
}

bool GGlueQuest::IsInvalidOwner() const
{
	return m_pOwner == NULL;
}

const string GGlueQuest::GetLuaTableName()
{
	GQuestInfo* pQuest = GetOwnerQuest()->GetInfo();
	return MAKE_LUATABLE_QUEST(pQuest ? pQuest->nID : 0);	
}

bool GGlueQuest::IsNull(void) const
{
	if (IsInvalidOwner())		return true;
	return false;
}

void GGlueQuest::UpdateQuestVar(int nVar)
{
	if (IsInvalidOwner())		return;

	gsys.pQuestSystem->GetQuestVarUpdater().Update(GetOwnerQuest()->GetOwner(), GetOwnerQuest()->GetInfo()->nID, nVar);
}

int  GGlueQuest::GetQuestVar(void)
{
	if (IsInvalidOwner())	return 0;
	
	return GetOwnerQuest()->GetQuestVar();
}
