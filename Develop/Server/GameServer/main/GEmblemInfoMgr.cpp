#include "StdAfx.h"
#include "GEmblemInfoMgr.h"
#include "GGlobal.h"
#include "GConditionsInfoMgr.h"
#include "CSEmblemInfo.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"
#include "AStlUtil.h"

GEmblemInfoMgr::GEmblemInfoMgr(void)
{
}

GEmblemInfoMgr::~GEmblemInfoMgr(void)
{
}

vector<const CSEmblemInfo*> GEmblemInfoMgr::GetKillEmblem( int nNPCID )
{
	KillEmblemMap::iterator it = m_mapKillEmblems.find(nNPCID);
	if (it == m_mapKillEmblems.end())
		return vector<const CSEmblemInfo*>();

	return it->second;
}

void GEmblemInfoMgr::OnLoadCompleted()
{
	for each (const EmblemMap::value_type& data in m_mapEmblems)
	{
		const CSEmblemInfo* pEmblemInfo = data.second;
		int nKillNPCID = GetKillNPCIDByCondition(pEmblemInfo->nConditionID);
		if (nKillNPCID == INVALID_NPC_ID)
			continue;

		MapSecondVectorInserter(m_mapKillEmblems, nKillNPCID, pEmblemInfo);
	}
}

int GEmblemInfoMgr::GetKillNPCIDByEmblem(int nEmblemID)
{
	const CSEmblemInfo* pEmblemInfo = Get(nEmblemID);
	if (!pEmblemInfo)
		return INVALID_NPC_ID;

	return GetKillNPCIDByCondition(pEmblemInfo->nConditionID);
}

int GEmblemInfoMgr::GetKillNPCIDByCondition(int nConditionID)
{
	GConditionsInfo* pConditionInfo = gmgr.pCondtionsInfoMgr->Get(nConditionID);
	DCHECK(pConditionInfo);
	if (!pConditionInfo)
		return INVALID_NPC_ID;

	if (!pConditionInfo->m_pConditionComposite)
		return INVALID_NPC_ID;

	if (pConditionInfo->m_pConditionComposite->GetType() != CCT_COMPOSITE)
		return INVALID_NPC_ID;

	if (pConditionInfo->m_pConditionComposite->m_vecConditionComponent.empty())
		return INVALID_NPC_ID;

	GCondition_Leaf* pConditionLeaf = dynamic_cast<GCondition_Leaf*>(pConditionInfo->m_pConditionComposite->m_vecConditionComponent[0]);
	if (!pConditionLeaf)
		return INVALID_NPC_ID;

	if (pConditionLeaf->m_nCT != CT_KILL)
		return INVALID_NPC_ID;

	if (pConditionLeaf->m_vecParam1.size() < 1)
		return INVALID_NPC_ID; // paramÀÌ ¾øÀ½

	int nKillNPCID = pConditionLeaf->m_vecParam1[0];
	return nKillNPCID;
}
