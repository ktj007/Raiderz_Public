#include "stdafx.h"
#include "GFieldFactory.h"
#include "GFieldMgr.h"
#include "GFieldInfoMgr.h"
#include "GTestSystem.h"
#include "GManagedFieldInfoMgr.h"
#include "GSharedField.h"
#include "GSharedFieldMaster.h"
#include "GServer.h"
#include "GMasterServerFacade.h"
#include "GTrialField.h"
#include "GInnRoomField.h"
#include "GDynamicFieldMaster.h"

bool GFieldFactory::CreateAllSharedField( GFieldInfoMgr* pFieldInfoMgr, GTestSystem* pTestSystem, GServer* pServer )
{
	MTimeChecker2 time_checker;
	time_checker.Start();

	GManagedFieldInfoMgr* pManagedFieldInfoMgr = gsys.pMasterServerFacade->GetManagedFieldInfoMgr();

	for (GFieldInfoMgr::iterator itor = pFieldInfoMgr->begin(); itor != pFieldInfoMgr->end(); ++itor)
	{
		GFieldInfo* pFieldInfo = (*itor).second;
		if (pFieldInfo->m_bDynamic) continue;
		if (pManagedFieldInfoMgr->Exist(pFieldInfo->m_nFieldID) == false) continue;

		bool bCreatedFieldMaster = (m_pFieldMgr->CreateSharedFieldMaster(pFieldInfo) != NULL);
		_ASSERT(bCreatedFieldMaster);
		if (!bCreatedFieldMaster)
		{
			mlog3("공유 필드 생성에 실패했습니다. (FieldID: %d)\n", pFieldInfo->m_nFieldID);
			return false;
		}

		// 기본 채널 생성
		for (int channel_id = 1; channel_id <= pFieldInfo->m_nDefaultChannelCount; channel_id++)
		{
			if (pManagedFieldInfoMgr->Exist(pFieldInfo->m_nFieldID, channel_id))
			{
				MUID uidNewField = pManagedFieldInfoMgr->GetFieldUID(pFieldInfo->m_nFieldID, channel_id);
				bool bCreatedField = (m_pFieldMgr->CreateSharedField(uidNewField, pFieldInfo->m_nFieldID, channel_id) != NULL);
				_ASSERT(bCreatedField);
				if (!bCreatedField)
				{
					mlog3("공유 필드 채널 생성에 실패했습니다. (FieldID: %d, ChannelID: $%d)\n", pFieldInfo->m_nFieldID, channel_id);
					return false;
				}
			}
		}

		pTestSystem->CreateMassSpawnTestFields(pFieldInfo);
	}

	dlog("%s - complete (elapsed: %.1f seconds)\n", __FUNCTION__, time_checker.Check() * 0.001f);

	return true;
}

GSharedField* GFieldFactory::CreateSharedField( MUID uidNewField, int nFieldID, CHANNELID nNewChannelID )
{
	return m_pFieldMgr->CreateSharedField(uidNewField, nFieldID, nNewChannelID);
}

GSharedFieldMaster* GFieldFactory::CreateSharedFieldMaster( GFieldInfo* pFieldInfo )
{
	return m_pFieldMgr->CreateSharedFieldMaster(pFieldInfo);
}

GTrialField* GFieldFactory::NewTrialField( MUID& uidNew, MUID uidGroup )
{
	return new GTrialFieldMLeaf(uidNew, uidGroup);
}

GInnRoomField* GFieldFactory::NewInnRoomField( MUID& uidNew, MUID uidGroup )
{
	return new GInnRoomFieldMLeaf(uidNew, uidGroup);
}

GDynamicFieldMaster* GFieldFactory::NewDynamicFieldMaster( const MUID& uidCreator, const DYNAMIC_FIELD_TYPE eType )
{
	return new GDynamicFieldMasterMLeaf(uidCreator, eType);
}

GSharedFieldMaster* GFieldFactory::NewSharedFieldMaster( GFieldInfo* pFieldInfo )
{
	return new GSharedFieldMasterMLeaf(pFieldInfo);
}