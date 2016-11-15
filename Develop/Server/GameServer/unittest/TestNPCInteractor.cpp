#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseMockLink.h"
#include "GNPCInteractor.h"
#include "GQuestInfo.h"
#include "GDialogInfo.h"
#include "GPlayerInteraction.h"
#include "GNPCInteraction.h"
#include "GNPCTarget.h"
#include "GQuestInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GInteractionSystem.h"
#include "GTestForward.h"
#include "GPlayerDoing.h"

SUITE(NPCInteractor)
{
	struct Fixture : public FBaseMockLink
	{
		Fixture()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = GUTHelper::NewEntityPlayer(m_pField);
			m_pNPC = m_Helper.NewEntityNPC(m_pField);
			m_pNPC->GetNPCStatus().nAttackable = NAT_NONE;

			m_pInteractionItem  = m_Helper.GiveNewItem(m_pPlayer);
			m_pInteractionItem->m_pItemData->m_ItemType = ITEMTYPE_USABLE;
			m_pInteractionItem->m_pItemData->m_nUsableType = USABLE_INTERACTION;

			m_pDialogInfo = test.dialog.NewDialogInfo();
			m_pQuestInfo = test.quest.NewQuestInfo();

			m_pQuestIElementInfo = test.interaction.GiveNewIElementInfoForClick(m_pNPC->GetNPCInfo(), IT_QUEST_BEGIN, m_pQuestInfo->nID);
			m_pDialogIElementInfo = test.interaction.GiveNewIElementInfoForItem(m_pNPC->GetNPCInfo(), m_pInteractionItem->m_pItemData->m_nID, IT_DIALOG, m_pDialogInfo->m_nID);
		}

		virtual ~Fixture()
		{
			m_pField->Destroy();
		}

		GTestMgrWrapper<GQuestInfoMgr>			m_QuestInfoMgrWrapper;
		GTestMgrWrapper<GDialogInfoMgr>			m_DialogInfoMgrWrapper;
		GTestMgrWrapper<GNPCInfoMgr>			m_NPCInfoMgr;		
		GUTHelper								m_Helper;

		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityNPC*			m_pNPC;
		GItem*				m_pInteractionItem;
		GQuestInfo*			m_pQuestInfo;
		GDialogInfo*		m_pDialogInfo;
		GNPCInteractionElementInfo*		m_pQuestIElementInfo;		
		GNPCInteractionElementInfo*		m_pDialogIElementInfo;
		GNPCInteractor		m_NPCInteractor;
	};

	namespace Interaction
	{		
		// 인터랙션 가능한 NPC가 아닐때
		TEST_FIXTURE(Fixture, NotInteracableNPC)
		{
			m_pNPC->GetNPCInfo()->bInteract = false;
			CHECK_EQUAL(false, m_NPCInteractor.Interaction(m_pPlayer, m_pNPC, m_pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_CLICK));
		}

		// 인터랙션 가능한 NPC이지만, 인터랙션이 비활성화 되었을때
		TEST_FIXTURE(Fixture, DisabledInteraction)
		{
			m_pNPC->GetNPCInteraction().Disable();
			CHECK_EQUAL(false, m_NPCInteractor.Interaction(m_pPlayer, m_pNPC, m_pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_CLICK));
		}

		// 인터랙션 가능한 거리가 아닐때
		TEST_FIXTURE(Fixture, FarDistance)
		{
			test.interaction.SetUninteractableDistacne(m_pNPC, m_pPlayer);

			CHECK_EQUAL(false, m_NPCInteractor.Interaction(m_pPlayer, m_pNPC, m_pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_CLICK));
		}

		TEST_FIXTURE(Fixture, Success)
		{
			MockLink* pLink = NewLink(m_pPlayer);
			CHECK_EQUAL(true, m_NPCInteractor.Interaction(m_pPlayer, m_pNPC, m_pNPC->GetNPCInfo()->m_InteractionInfo.m_vecNPCIEInfo, ITRT_CLICK));

			CHECK_EQUAL(true, m_pPlayer->GetDoing().IsNowInteracting());
			CHECK_EQUAL(ITAT_NPC, m_pPlayer->GetInteraction().GetITargetType());
			CHECK_EQUAL(true, m_pNPC->GetNPCInteraction().IsInteractingPlayer(m_pPlayer));

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_NPCINTERACTION_INTERACTION, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(m_pNPC->GetUID(), pLink->GetParam<MUID>(0, 0));
			CHECK(m_pNPC->GetNPCInfo()->m_strOpeningStatement == pLink->GetParam<const wchar_t*>(0, 1));
			vector<TD_INTERACTION_ELEMENT> vecTDInteractionElement;
			pLink->GetCommand(0).GetBlob(vecTDInteractionElement, 2);
			TD_INTERACTION_ELEMENT tdInteractionElement = vecTDInteractionElement[0];
			CHECK_EQUAL(tdInteractionElement.nIElementID, m_pQuestIElementInfo->m_nID);
			GDialogInfo* pDialogInfo = gmgr.pDialogInfoMgr->Get(m_pQuestInfo->nGiveDialogID);
			ASSERT_CHECK(NULL != pDialogInfo);
			CHECK(0 == wcscmp(tdInteractionElement.szIEText, pDialogInfo->m_strText.c_str()));

			// 인터랙션한 플레이어를 타게팅했는지 확인
			CHECK_EQUAL(m_pPlayer->GetUID(), m_pNPC->GetNPCTarget().GetTargetUID());
			CHECK_EQUAL(m_pPlayer->GetUID(), m_pNPC->GetNPCTarget().GetTarget()->GetUID());
		}
	}

	namespace InteractionElement
	{
		struct FInteractionElement : public Fixture
		{
			FInteractionElement()
			{
				m_NPCInteractor.InteractionByClick(m_pPlayer, m_pNPC);	
			}

			virtual ~FInteractionElement()
			{

			}
		};

		// 요청한 ID를 가지는 선택한 IElementInfo가 없을 경우
		TEST_FIXTURE(FInteractionElement, SelectableIElementInfoIsNull)
		{			
			CHECK_EQUAL(false, m_NPCInteractor.InteractionElement(m_pPlayer, m_pNPC, INVALID_ID));
		}
		
		// 인터랙션 가능한 거리가 아닐때
		TEST_FIXTURE(FInteractionElement, FarDistance)
		{
			test.interaction.SetUninteractableDistacne(m_pNPC, m_pPlayer);

			CHECK_EQUAL(false, m_NPCInteractor.InteractionElement(m_pPlayer, m_pNPC, m_pQuestIElementInfo->m_nID));
		}

		TEST_FIXTURE(FInteractionElement, Success)
		{
			CHECK_EQUAL(true, m_NPCInteractor.InteractionElement(m_pPlayer, m_pNPC, m_pQuestIElementInfo->m_nID));
		}
	}
}
