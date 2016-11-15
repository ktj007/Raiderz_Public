#include "stdafx.h"
#include "MockField.h"
#include "MockLink.h"
#include "GTestForward.h"
#include "GDialogSystem.h"
#include "GDialogInfo.h"
#include "GItemAddRemoveInfo.h"
#include "GItemHolder.h"
#include "GPlayerInteraction.h"


SUITE(DialogSystem)
{
	struct Fixture 
	{
		Fixture()
		{
			m_pField = test.field.DefaultMockField();
			m_pPlayer = test.player.NewPlayer(m_pField);
			m_pNPC = test.npc.SpawnNPC(m_pField);

			m_pDialogInfo = test.dialog.NewDialogInfo();
			m_pSayInfo = test.dialog.NewDialogSayInfo();
			m_pDialogInfo->m_pDialogSayInfo = m_pSayInfo;
			m_pSelectInfo = m_pSayInfo->m_vecDialogSelectInfo.front();

			m_pLink = test.network.NewLink(m_pPlayer);
			m_pLink->ResetCommands();
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GEntityNPC*		m_pNPC;
		MockLink*		m_pLink;

		GDialogInfo*	m_pDialogInfo;
		GDialogSayInfo* m_pSayInfo;
		GDialogSelectInfo* m_pSelectInfo;
		DECLWRAPPER_NPCMgr;
	};

	// exit값이 0이면 아이템 추가 제거가 일어나지 않는다.
	TEST_FIXTURE(Fixture, Select_Exit0_ApplyItemAddRemove)
	{
		m_pSelectInfo->m_pItemAddRemoveInfo = new GItemAddRemoveInfo;
		GItemData* pItemData = test.item.NewItemData();		
		int nAddAmount = 1;
		m_pSelectInfo->m_pItemAddRemoveInfo->InsertItemAddInfo(pItemData->m_nID, nAddAmount);
		m_pSelectInfo->m_nExit = 0;
		m_pPlayer->GetInteraction().BeginDialog(m_pDialogInfo->m_nID, m_pDialogInfo->m_pDialogSayInfo);

		int nBeforeAmount = m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID);
		gsys.pDialogSystem->Select(m_pPlayer, 0);

		CHECK_EQUAL(nBeforeAmount, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
	}	

	// exit값이 0이 아니면 아이템 추가 제거가 일어난다.
	TEST_FIXTURE(Fixture, Select_ExitNotZero_ApplyItemAddRemove)
	{
		m_pSelectInfo->m_pItemAddRemoveInfo = new GItemAddRemoveInfo;
		GItemData* pItemData = test.item.NewItemData();	
		int nAddAmount = 1;
		m_pSelectInfo->m_pItemAddRemoveInfo->InsertItemAddInfo(pItemData->m_nID, nAddAmount);
		m_pSelectInfo->m_nExit = 1;
		m_pPlayer->GetInteraction().BeginDialog(m_pDialogInfo->m_nID, m_pDialogInfo->m_pDialogSayInfo);

		int nBeforeAmount = m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID);
		gsys.pDialogSystem->Select(m_pPlayer, 0);

		CHECK_EQUAL(nBeforeAmount+nAddAmount, m_pPlayer->GetItemHolder()->GetItemCount(SLOTTYPE_INVENTORY, pItemData->m_nID));
	}	
}
