#include "stdafx.h"
#include "GSoulBindingInfo.h"
#include "GSoulBindingInfoMgr.h"
#include "CCommandResultTable.h"
#include "GTestWrapper.h"
#include "MockPlayerObject.h"
#include "SUnitTestUtil.h"
#include "MockField.h"
#include "GFieldMgr.h"
#include "GSharedFieldMaster.h"
#include "GPlayerObjectManager.h"
#include "GFieldInfoMgr.h"
#include "GRebirthSystem.h"
#include "GPlayerSystem.h"

SUITE(PlayerSystem)
{
	struct FSaveSoulBinding
	{
		FSaveSoulBinding()
		{
			m_pPlayerObject = new MockPlayerObject(SUnitTestUtil::NewUID());
			m_pPlayerObject->Create();
			gmgr.pPlayerObjectManager->AddPlayer(m_pPlayerObject);
			m_pPlayer = m_pPlayerObject->GetEntity();

			m_pSoulBindingInfo = new GSoulBindingInfo();
			m_pSoulBindingInfo->m_nID = 1;
			m_pSoulBindingInfo->m_pMarkerInfo = new MARKER_INFO();
			gmgr.pSoulBindingInfoMgr->Insert(m_pSoulBindingInfo);
		}

		virtual ~FSaveSoulBinding()		
		{
			gmgr.pPlayerObjectManager->DeleteAllPlayer();

			SAFE_DELETE(m_pSoulBindingInfo->m_pMarkerInfo);
		}

		GTestMgrWrapper<GSoulBindingInfoMgr>	m_SoulBindingInfoMgrWrapper;
		GTestMgrWrapper<GFieldInfoMgr>			m_FieldInfoMgrWrapper;
		GSoulBindingInfo*	m_pSoulBindingInfo;
		MockPlayerObject*	m_pPlayerObject;
		GEntityPlayer*		m_pPlayer;
	};

	TEST_FIXTURE(FSaveSoulBinding, Fail_InvalidPCPointer)
	{
		CHECK_EQUAL(false, gsys.pPlayerSystem->SaveSoulBinding(NULL, m_pSoulBindingInfo->m_nID));
	}

	TEST_FIXTURE(FSaveSoulBinding, Fail_InvalidSoulBindingID)
	{
		CHECK_EQUAL(false, gsys.pPlayerSystem->SaveSoulBinding(m_pPlayer, 0));
	}

	TEST_FIXTURE(FSaveSoulBinding, Success_Normal)
	{
		CHECK_EQUAL(true, gsys.pPlayerSystem->SaveSoulBinding(m_pPlayer, m_pSoulBindingInfo->m_nID));
		CHECK_EQUAL(m_pSoulBindingInfo->m_nID, m_pPlayer->GetPlayerInfo()->nSoulBindingID);
	}	
}

	