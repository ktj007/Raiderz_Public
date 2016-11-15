#include "stdafx.h"
#include "GTestWrapper.h"
#include "GTestForward.h"
#include "MockField.h"
#include "GPlayerNPCIconSender.h"
#include "GDialogInfo.h"
#include "GQuestInfo.h"
#include "AStlUtil.h"
#include "GQuestInfoMgr.h"
#include "GNPCIconSelector.h"

SUITE(PlayerNPCIconSender)
{
	struct Fixture
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPCIConSender = &m_pPlayer->GetNPCIconSender();
			m_pNPC = test.npc.SpawnNPC(m_pField);
			m_pNPCIConSender->ClearNPCIconData();
		}

		~Fixture()
		{
			test.network.ClearLinks();
			m_pField->Destroy();	
		}

		DECLWRAPPER_NPCInfoMgr;
		GPlayerNPCIconSender* m_pNPCIConSender;
		MockField*			m_pField;
		GEntityPlayer*		m_pPlayer;
		GEntityNPC*			m_pNPC;
	};

	TEST_FIXTURE(Fixture, SendByFindEnemy)
	{
		MockLink* pLink = test.network.NewLink(m_pPlayer);

		m_pNPCIConSender->SendByFindEnemy(m_pNPC);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPCINTERACTION_ICON, pLink->GetCommand(0).GetID());

		vector<TD_NPC_ICON> vecNPCIcon;
		pLink->GetCommand(0).GetBlob(vecNPCIcon, 0);
		CHECK_EQUAL(m_pNPC->GetUIID(), vecNPCIcon[0].m_nUIID);
		CHECK_EQUAL(NIT_COMBAT, vecNPCIcon[0].m_nIcon);		
	}

	TEST_FIXTURE(Fixture, SendByDisableNPCInteraction)
	{
		MockLink* pLink = test.network.NewLink(m_pPlayer);

		m_pNPCIConSender->SendByDisableNPCInteraction(m_pNPC);

		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_NPCINTERACTION_ICON, pLink->GetCommand(0).GetID());
		vector<TD_NPC_ICON> vecNPCIcon;
		pLink->GetCommand(0).GetBlob(vecNPCIcon, 0);
		CHECK_EQUAL(m_pNPC->GetUIID(), vecNPCIcon[0].m_nUIID);
		CHECK_EQUAL(NIT_NONE, vecNPCIcon[0].m_nIcon);
	}

	TEST_FIXTURE(Fixture, CollectEssentialNPCUID)
	{
		GNPCInfo* pNPCInfo = test.npc.NewNPCInfo();
		GDialogInfo* pDialogInfo = test.dialog.NewDialogInfo();
		test.interaction.GiveNewIElementInfoForClick(pNPCInfo, IT_DIALOG, pDialogInfo->m_nID);
		SPAWN_INFO* pSpawnInfo = test.field.InsertNewSpawnInfo(m_pField->GetInfo(), 0, pNPCInfo->nID);

		GNPCInfo* pEssentialNPCInfo = test.npc.NewNPCInfo();
		GQuestInfo* pQuestInfo = test.quest.NewQuestInfo();
		test.interaction.GiveNewIElementInfoForClick(pEssentialNPCInfo, IT_QUEST_BEGIN, pQuestInfo->nID);		
		SPAWN_INFO* pEssentialSpawnInfo = test.field.InsertNewSpawnInfo(m_pField->GetInfo(), 0, pEssentialNPCInfo->nID);

		GFieldInfo* pFieldInfo = const_cast<GFieldInfo*>(m_pField->GetInfo());
		pFieldInfo->Cooking();
		gmgr.pQuestInfoMgr->Cooking();

		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField, pNPCInfo->nID);
		GEntityNPC* pEssentialNPC = test.npc.SpawnNPC(m_pField, pEssentialNPCInfo->nID);

		vector<MUID> vecEssentialNPCUID;
		m_pPlayer->GetNPCIconSender().CollectEssentialNPCUID(vecEssentialNPCUID);

		CHECK(IsExist(vecEssentialNPCUID, pEssentialNPC->GetUID()));
		CHECK(IsNotExist(vecEssentialNPCUID, pNPC->GetUID()));
	}

	TEST_FIXTURE(Fixture, FilterSendedIcon)
	{
		GEntityNPC* pSendedNPC = test.npc.SpawnNPC(m_pField);
		GEntityNPC* pNPC = test.npc.SpawnNPC(m_pField);
		NPC_ICON sendedNPCIcon(pSendedNPC, NIT_QUEST_BEGIN);
		NPC_ICON NPCIcon(pNPC, NIT_QUEST_BEGIN);
		m_pPlayer->GetNPCIconSender().FilterSendedIcon(vector<NPC_ICON>(1, sendedNPCIcon), vector<NPC_ICON>());

		vector<NPC_ICON> vecNPCIcon;
		vecNPCIcon.push_back(sendedNPCIcon);
		vecNPCIcon.push_back(NPCIcon);
		vector<NPC_ICON> vecFilteredNPCIcon;
		m_pPlayer->GetNPCIconSender().FilterSendedIcon(vecNPCIcon, vecFilteredNPCIcon);

		CHECK(NPCIcon == vecFilteredNPCIcon.front());
		CHECK(1 == vecFilteredNPCIcon.size());
	}
}