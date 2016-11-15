#include "StdAfx.h"
#include "GTrainingInfo.h"

GTrainingInfo::GTrainingInfo()
: m_nID(0)
, m_pDialogInfo(NULL)
, nTrainingType(TRA_NONE)
{
}

GTrainingInfo::~GTrainingInfo()
{
}

GConditionsInfo* GTrainingInfo::GetConditionsInfo(GTalentInfo* pTalentInfo)
{
	map<GTalentInfo*, GConditionsInfo*>::iterator itor = m_mapConditionsTalent.find(pTalentInfo);

	if (m_mapConditionsTalent.end() == itor) return NULL;

	return (*itor).second;
}