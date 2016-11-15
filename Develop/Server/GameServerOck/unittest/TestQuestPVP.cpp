#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GQPEventInfo.h"
#include "CSDef_QuestPVP.h"
#include "GQuestPVP.h"
#include "GItemHolder.h"
#include "GFieldAttrGrid.h"

SUITE(QuestPVP)
{
	struct Fixture : FBaseGame
	{
		Fixture()
		{
			pPlayer2 = test.player.NewPlayer(m_pField);
			pLink2 = test.network.NewLink(pPlayer2);

			pNPCInfo = test.npc.NewNPCInfo();
			pNPCInfo2 = test.npc.NewNPCInfo();
			pNPCInfo3 = test.npc.NewNPCInfo();
			pTeamOneRewardItemData = test.item.NewItemData();
			nTeamOneRewardMoney = 3000;
			nTeamOneRewardXP = 300;

			pTeamTwoRewardItemData = test.item.NewItemData();
			nTeamTwoRewardMoney = 4000;
			nTeamTwoRewardXP = 400;

			m_pField->GetSpawn()->Fini();

			InitQPEventInfo();
			InitPVPAreaInfo();			

			m_pField->GetSpawn()->Init();
			m_pLink->ResetCommands();
			m_pLink->AddIgnoreID(MC_FIELD_IN_NPC);
			m_pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);	
			m_pLink->AddIgnoreID(MC_CHAR_UPDATE_MONEY);	
			m_pLink->AddIgnoreID(MC_ITEM_ADD);			
			m_pLink->AddIgnoreID(MC_CHAR_REWARD_EXP);
			pLink2->ResetCommands();
			pLink2->AddIgnoreID(MC_FIELD_IN_NPC);
			pLink2->AddIgnoreID(MC_NPCINTERACTION_ICON);	
			pLink2->AddIgnoreID(MC_CHAR_UPDATE_MONEY);	
			pLink2->AddIgnoreID(MC_ITEM_ADD);			
			pLink2->AddIgnoreID(MC_CHAR_REWARD_EXP);
		}

		void InitPVPAreaInfo() 
		{
			PVP_AREA_INFO infoPVP;
			infoPVP.rcArea = MRect(-500, -500, 500, 500);
			GFieldInfo* pFieldInfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
			pFieldInfo->m_PvPAreas.push_back(infoPVP);
			pFieldInfo->GetAttrGrid()->Init();
			pFieldInfo->Cooking();
			test.player.SetMockPlayerPVPArea_LocateAndTeamOne(m_pPlayer);			
			test.player.SetMockPlayerPVPArea_LocateAndTeamTwo(pPlayer2);			
		}

		void InitQPEventInfo() 
		{
			pInfo = test.questpvp.NewInfo();
			test.questpvp.InsertNotice(pInfo->m_BeginInfo);
			test.questpvp.InsertSpawnInfo(pInfo->m_BeginInfo, m_pField->GetInfo(), pNPCInfo->nID);

			pTeamOneEndInfo = test.questpvp.InsertEndInfo(pInfo, QT_ONE);
			test.questpvp.InsertNotice(pTeamOneEndInfo);
			test.questpvp.InsertSpawnInfo(pTeamOneEndInfo, m_pField->GetInfo(), pNPCInfo2->nID);
			test.questpvp.InsertDespawnNPCID(pTeamOneEndInfo, pNPCInfo->nID);
			pTeamOneEndInfo->m_nMoney = nTeamOneRewardMoney;
			pTeamOneEndInfo->m_nXP = nTeamOneRewardXP;
			test.questpvp.InsertItemInfo(pTeamOneEndInfo, pTeamOneRewardItemData->m_nID);

			pTeamTwoEndInfo = test.questpvp.InsertEndInfo(pInfo, QT_TWO);	
			test.questpvp.InsertNotice(pTeamTwoEndInfo);
			test.questpvp.InsertSpawnInfo(pTeamTwoEndInfo, m_pField->GetInfo(), pNPCInfo3->nID);
			test.questpvp.InsertDespawnNPCID(pTeamTwoEndInfo, pNPCInfo->nID);
			pTeamTwoEndInfo->m_nMoney = nTeamTwoRewardMoney;
			pTeamTwoEndInfo->m_nXP = nTeamTwoRewardXP;
			test.questpvp.InsertItemInfo(pTeamTwoEndInfo, pTeamTwoRewardItemData->m_nID);			
		}

		~Fixture()
		{
			m_pField->GetSpawn()->Fini();
			const_cast<GFieldInfo*>(m_pField->GetInfo())->Reset();
		}

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_NPCInfoMgr;

		GEntityPlayer*	pPlayer2;
		MockLink*		pLink2;

		GQPEventInfo*	pInfo;
		GQPEEndInfo*	pTeamOneEndInfo;
		GQPEEndInfo*	pTeamTwoEndInfo;
		GNPCInfo*		pNPCInfo;
		GNPCInfo*		pNPCInfo2;
		GNPCInfo*		pNPCInfo3;

		int				nTeamOneRewardMoney;
		int				nTeamOneRewardXP;
		GItemData*		pTeamOneRewardItemData;

		int				nTeamTwoRewardMoney;
		int				nTeamTwoRewardXP;
		GItemData*		pTeamTwoRewardItemData;
	};

	TEST_FIXTURE(Fixture, BeginEvent)
	{
		m_pField->GetQuestPVP()->BeginEvent(pInfo->m_nID);

		CHECK_EQUAL(MC_MSG_RES, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(1, m_pField->GetNPCQty(pNPCInfo->nID));

		CHECK_EQUAL(MC_QUESTPVP_BEGIN_EVENT, m_pLink->GetCommand(1).GetID());		
	}

	TEST_FIXTURE(Fixture, EndEvent_WinnerIsTeamOne)
	{
		m_pField->GetQuestPVP()->EndEvent(pInfo->m_nID, QT_ONE);

		CHECK_EQUAL(MC_MSG_RES, m_pLink->GetCommand(0).GetID());
		CHECK_EQUAL(1, m_pField->GetNPCQty(pNPCInfo2->nID));
		CHECK_EQUAL(0, m_pField->GetNPCQty(pNPCInfo->nID));
		CHECK_EQUAL(nTeamOneRewardMoney, m_pPlayer->GetMoney());
		CHECK_EQUAL(nTeamOneRewardXP, m_pPlayer->GetPlayerInfo()->nXP);
		CHECK_EQUAL(1, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pTeamOneRewardItemData->m_nID));


		CHECK_EQUAL(MC_QUESTPVP_END_EVENT, m_pLink->GetCommand(1).GetID());		
	}

	TEST_FIXTURE(Fixture, EndEvent_WinnerIsTeamTwo)
	{
		m_pField->GetQuestPVP()->EndEvent(pInfo->m_nID, QT_TWO);

		CHECK_EQUAL(MC_MSG_RES, pLink2->GetCommand(0).GetID());
		CHECK_EQUAL(1, m_pField->GetNPCQty(pNPCInfo3->nID));
		CHECK_EQUAL(0, m_pField->GetNPCQty(pNPCInfo->nID));
		CHECK_EQUAL(nTeamTwoRewardMoney, pPlayer2->GetMoney());
		CHECK_EQUAL(nTeamTwoRewardXP, pPlayer2->GetPlayerInfo()->nXP);
		CHECK_EQUAL(1, pPlayer2->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pTeamTwoRewardItemData->m_nID));


		CHECK_EQUAL(MC_QUESTPVP_END_EVENT, pLink2->GetCommand(1).GetID());		
	}
}