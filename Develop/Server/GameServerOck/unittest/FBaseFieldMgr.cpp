#include "stdafx.h"
#include "FBaseFieldMgr.h"
#include "GFieldFactory.h"
#include "GFieldInfoMgr.h"


GSharedField* FBaseFieldMgr::AddSharedField(MUID uidNewField, int nFieldID, int nChannelID)
{
	GSharedFieldMaster* pFieldMaster = gmgr.pFieldMgr->GetSharedFieldMaster(nFieldID);
	if (NULL == pFieldMaster)
	{
		GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
		gmgr.pFieldMgr->GetFactory()->CreateSharedFieldMaster(pFieldInfo);
	}

	return gmgr.pFieldMgr->GetFactory()->CreateSharedField(uidNewField, nFieldID, nChannelID);
}
