#include "StdAfx.h"
#include "GUTHelper_NPC.h"
#include "SUnitTestUtil.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GNPCInfoMgr.h"
#include "MockField.h"
#include "GModuleAI.h"

GNPCInfo* GUTHelper_NPC::NewNPCInfo( int nNPCId/*=INVALID_ID*/ )
{
	if (nNPCId == INVALID_ID)
	{
		nNPCId = SUnitTestUtil::NewID();
	}

	GNPCInfo* pNPCInfo = NULL;
	
	pNPCInfo = gmgr.pNPCInfoMgr->Find(nNPCId);

	if (!pNPCInfo)
	{
		pNPCInfo = new GNPCInfo();
		pNPCInfo->nID = nNPCId;
		pNPCInfo->fSpeed = 100;
		pNPCInfo->fRunSpeed = 300;
		pNPCInfo->nAA = NAAT_ALWAYS;
		pNPCInfo->nInitAttackable = NAT_ALWAYS;

		pNPCInfo->Cooking();

		gmgr.pNPCInfoMgr->Insert(pNPCInfo);
	}
	
	return pNPCInfo;
}

GEntityNPC* GUTHelper_NPC::SpawnNPC(MockField* pField, int nNPCId, const vec3& vPos)
{
	GNPCInfo* pNPCInfo = NewNPCInfo(nNPCId);
	return pField->SpawnTestNPC(pNPCInfo, vPos);
}

void GUTHelper_NPC::MakeCombat(GEntityNPC* pNPC, GEntityActor* pOpponent)
{
	VALID(pNPC);
	VALID(pOpponent);

	pNPC->doAttack(pOpponent->GetUID(), 0);
}

void GUTHelper_NPC::MakeCombat(GEntityNPC* pNPC)
{
	VALID(pNPC);

	pNPC->doAttack(pNPC->GetUID(), 0);
}

GNPCInfo* GUTHelper_NPC::NewNPCInfo_DeulFlag()
{
	GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(DUEL_FLAG_NPCID);
	if (NULL == pNPCInfo)
	{
		pNPCInfo = NewNPCInfo(DUEL_FLAG_NPCID);
		pNPCInfo->nInitAttackable = NAT_NONE;
	}
	
	return pNPCInfo;
}