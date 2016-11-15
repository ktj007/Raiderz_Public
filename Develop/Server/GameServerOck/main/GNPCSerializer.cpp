#include "StdAfx.h"
#include "GNPCSerializer.h"
#include "GEntityNPC.h"
#include "GNPCMgr.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GField.h"

GNPCSerializer::GNPCSerializer(void)
{
}

GNPCSerializer::~GNPCSerializer(void)
{
}

void GNPCSerializer::SaveFromInstance( GEntityNPC* pNPC, bool bDelete )
{
	VALID(pNPC);
	VALID(pNPC->GetField());
	VALID(pNPC->GetNPCInfo());
	Info info;
	info.nNPCID = pNPC->GetNPCInfo()->nID;
	info.uidField = pNPC->GetFieldUID();
	info.vPos = pNPC->GetPos();
	info.vDir = pNPC->GetDir();
	info.vFacingDir = pNPC->GetFacingDir();
	info.pSpawnInfo = pNPC->GetSpawnInfo();
	m_vecNPCs.push_back(info);

	if (bDelete)
	{
		pNPC->DeleteMe();
		pNPC->GetField()->Update(0.0f);
	}
}

void GNPCSerializer::SaveFromManager(int nNPCID, bool bDelete)
{
	MAP_NPC mapNPC = gmgr.pNPCMgr->GetNPCMap();
	MAP_NPC::iterator iter = mapNPC.begin();
	for (; iter != mapNPC.end(); ++iter)
	{
		GEntityNPC* pNPC = iter->second;
		if (!pNPC)						
			continue;
		if (pNPC->GetID() != nNPCID)	
			continue;

		gmgr.pNPCMgr->RemoveNPC(pNPC);
		SaveFromInstance(pNPC, bDelete);
	}
}

void GNPCSerializer::RestoreAll()
{
	for each (Info each in m_vecNPCs)
	{
		GField* pField = 
			gmgr.pFieldMgr->GetField(each.uidField);
		if (!pField)
			continue;

		pField->SpawnNPC(each.nNPCID, each.pSpawnInfo, each.vPos, each.vDir);
	}
}
