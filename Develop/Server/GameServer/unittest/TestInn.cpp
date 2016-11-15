#include "stdafx.h"
#include "GUTHelper.h"
#include "MockLink.h"
#include "MockField.h"
#include "GInteractionSystem.h"
#include "GInnSystem.h"
#include "CCommandTable.h"
#include "GFatigueSystem.h"
#include "GFieldSystem.h"
#include "GDynamicFieldMaster.h"
#include "GDynamicField.h"
#include "GInnRoomInfoMgr.h"
#include "GInnRoomInfo.h"
#include "GTestForward.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "GDialogInfo.h"
#include "GPlayerField.h"
#include "FBaseMockLink.h"
#include "GNPCInteractor.h"
#include "GTestForward.h"
#include "GUTHelper_Field.h"
#include "CCommandResultTable.h"
#include "CSDef_Fatigue.h"

SUITE(InnSystem)
{
	struct FInnSystem : public FBaseMockLink
	{
		FInnSystem()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pNPC->GetNPCStatus().nAttackable = NAT_NONE;
		}

		virtual ~FInnSystem()
		{
			m_pField->Destroy();
		}

		DECLWRAPPER_TalentMgr;
		GUTHelper m_Helper;
		MockField* m_pField;
		GEntityPlayer* m_pPlayer;
		GEntityNPC* m_pNPC;
	};	

	
	namespace Sleep
	{
		struct FSleep : FInnSystem
		{
			FSleep()
			{
				GInnRoomInfo* pInnRoomInfo = m_Helper.NewInnRoomInfo();
				pInnRoomInfo->m_nType = IRT_NORMAL;
				m_pPlayer->GetPlayerInfo()->nInnRoomID = pInnRoomInfo->m_nID;

				m_pIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_INN_SLEEP);
				m_pIElementInfo->m_vecAct.push_back(test.dialog.NewDialogInfo()->m_nID);

				gsys.pInteractionSystem->GetNPCInteractor().InteractionByClick(m_pPlayer, m_pNPC);
			}

			GNPCInteractionElementInfo* m_pIElementInfo;
		};

		TEST_FIXTURE(FSleep, BeginSleep)
		{
			MockLink* pLink = NewLink(m_pPlayer);
			gsys.pInnsystem->BeginSleep(m_pPlayer);

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_INN_BEGIN_SLEEP, pLink->GetCommand(0).GetID());
		}

		/*namespace EndSleep
		{
			TEST_FIXTURE(FSleep, NormalRoom)
			{
				GInnRoomInfo* pInnRoomInfo = m_Helper.NewInnRoomInfo();
				pInnRoomInfo->m_nType = IRT_NORMAL;
				m_pPlayer->GetPlayerInfo()->nInnRoomID = pInnRoomInfo->m_nID;

				gsys.pInnsystem->BeginSleep(m_pPlayer);
				
				MockLink* pLink = NewLink(m_pPlayer);
				m_pPlayer->Update(SLEEP_ANI_TIME);
				CHECK_EQUAL(SET_FATIGUE_ROOM_NORMAL, m_pPlayer->GetPlayerInfo()->nFatigueQuantity);

				CHECK_EQUAL(3, pLink->GetCommandCount());
				CHECK_EQUAL(MC_FATIGUE_UPDATE, pLink->GetCommand(0).GetID());
				CHECK_EQUAL(FT_GOOD, pLink->GetParam<int>(0, 0));									
				CHECK_EQUAL(MC_INN_END_SLEEP, pLink->GetCommand(1).GetID());
				CHECK_EQUAL(MC_NPCINTERACTION_END, pLink->GetCommand(2).GetID());
			}

			TEST_FIXTURE(FSleep, SpecialRoom)
			{
				GInnRoomInfo* pInnRoomInfo = m_Helper.NewInnRoomInfo();
				pInnRoomInfo->m_nType = IRT_BASIC;
				m_pPlayer->GetPlayerInfo()->nInnRoomID = pInnRoomInfo->m_nID;
				
				gsys.pInnsystem->BeginSleep(m_pPlayer);

				MockLink* pLink = NewLink(m_pPlayer);
				m_pPlayer->Update(SLEEP_ANI_TIME);
				CHECK_EQUAL(SET_FATIGUE_ROOM_SPECIAL, m_pPlayer->GetPlayerInfo()->nFatigueQuantity);

				CHECK_EQUAL(3, pLink->GetCommandCount());
				CHECK_EQUAL(MC_FATIGUE_UPDATE, pLink->GetCommand(0).GetID());
				CHECK_EQUAL(FT_VERY_GOOD, pLink->GetParam<int>(0, 0));					
				CHECK_EQUAL(MC_INN_END_SLEEP, pLink->GetCommand(1).GetID());
				CHECK_EQUAL(MC_NPCINTERACTION_END, pLink->GetCommand(2).GetID());				
			}
		}*/
	}	
}

SUITE(InnRoomField)
{
	// 플레이어가 모두 나가면 여관방 필드는 자동 소멸 된다.
	TEST(DestroyWhenNobodyExist)
	{
		GTestMgrWrapper<GTalentInfoMgr> talentInfoMgr;
		GTestMgrWrapper<GFieldMgr> fieldMgr;

		DECLWRAPPER_TalentMgr;
		GUTHelper helper;

		GSharedField* pSharedField = GUTHelper_Field::NewSharedField(GUTHelper_Field::NewFieldInfo(), 1);
		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pSharedField, vec3(1000, 1000, 1000));

		FIELD_GROUP_INFO* pFieldGroupInfo = GUTHelper_Field::NewFieldGroupInfo();
		GInnRoomInfo* pInnRoomInfo = helper.NewInnRoomInfo();
		pInnRoomInfo->m_nType = IRT_NORMAL;
		pInnRoomInfo->m_nFieldGroupID = pFieldGroupInfo->nID;
		pPlayer->GetPlayerInfo()->nInnRoomID = pInnRoomInfo->m_nID;
		
		// 여관 입장
		pPlayer->GetPlayerField().GateToInnRoom();
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		MUID uidFieldGroup = pPlayer->GetPlayerField().GetDynamicFieldGroupUID();

		// 여관 퇴장 및 공유 필드 입장
		pPlayer->GetPlayerField().Gate(pSharedField->GetInfo()->m_nFieldID, vec3(1000, 1000, 1000), vec3(0,1,0), false);
		pPlayer->GetPlayerField().GetFieldEntry().OnEnterFieldComplete();
		CHECK_EQUAL(pPlayer->GetField()->GetUID(), pSharedField->GetUID());

		// 여관 파괴 변경: 퇴장 시 파괴에서, 업데이트 중 입장한 플레이어가 없으면 제거
		gmgr.pFieldMgr->Update(0.1f);	// Create SelfDestructor
		gmgr.pFieldMgr->Update(0.1f);	// Update SelfDestructor 

		CHECK(NULL == gmgr.pFieldMgr->GetDynamicFieldMaster(uidFieldGroup));
	}
}





