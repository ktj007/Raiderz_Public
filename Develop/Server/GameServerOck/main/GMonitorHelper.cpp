#include "stdafx.h"
#include "GMonitorHelper.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "GGlobal.h"
#include "GField.h"
#include "GFieldMgr.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"

uint32 GMonitorHelper::GetTotalNPCCount()
{
	// 해당 필드 NPC 수
	uint32 nAllNPCCount = 0;
	for (GFieldMgr::FIELD_BY_UID_MAP::iterator itor = gmgr.pFieldMgr->GetFieldMap().begin();
		itor != gmgr.pFieldMgr->GetFieldMap().end(); ++itor)
	{
		GField* pField = (*itor).second;
		int count = pField->GetEntityQty(ETID_NPC);
		nAllNPCCount += count;
	}
	return nAllNPCCount;
}