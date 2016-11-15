#include "stdafx.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "CCommandTable.h"
#include "MockLink.h"
#include "GEntityNPC.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"

struct FTalentDrain : public FBaseMockLink
{
	FTalentDrain()
	{
		pField = GUTHelper_Field::DefaultMockField();
		pPlayer = GUTHelper::NewEntityPlayer(pField);		 
		pNPC = helper.NewEntityNPC(pField);
		pTalentInfo = helper.NewTalentInfo();
		pTalentInfo->m_nMinDamage = 10;
		pTalentInfo->m_nMaxDamage = 10;
		pTalentInfo->m_Drain.m_fRate = 1.0f;
	}

	~FTalentDrain()
	{
		pField->Destroy();	

	}

	GUTHelper		helper;
	MockField*		pField;
	GEntityPlayer*	pPlayer;
	GEntityNPC*		pNPC;
	GTalentInfo*	pTalentInfo;
};

SUITE(TalentDrain)
{
	namespace DrainType
	{
		TEST_FIXTURE(FTalentDrain, HP)
		{
			pTalentInfo->m_Drain.m_nType = TALENT_DRAIN_HP;
			pPlayer->SetHP(1);

			int nBeforeHP = pPlayer->GetHP();
			MockLink* pLink = NewLink(pPlayer);
			pLink->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
			pLink->AddIgnoreID(MC_CHAR_DEL_ENEMYNPC);
			pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
			pNPC->doTryHit(pPlayer, pTalentInfo);
			int nAfterHP = pPlayer->GetHP();

			CHECK_EQUAL(nBeforeHP+pTalentInfo->m_nMinDamage, nAfterHP);
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_TALENT_DRAIN, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 0));
			CHECK(pTalentInfo->m_Drain.m_nType == pLink->GetParam<unsigned char>(0, 1));
			CHECK_EQUAL(pTalentInfo->m_nMinDamage, pLink->GetParam<int>(0, 2));
		}

		TEST_FIXTURE(FTalentDrain, EN)
		{
			pTalentInfo->m_Drain.m_nType = TALENT_DRAIN_EN;
			pPlayer->SetEN(1);

			int nBeforeEN = pPlayer->GetEN();
			MockLink* pLink = NewLink(pPlayer);
			pLink->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
			pLink->AddIgnoreID(MC_CHAR_DEL_ENEMYNPC);
			pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
			pNPC->doTryHit(pPlayer, pTalentInfo);
			int nAfterEN = pPlayer->GetEN();

			CHECK_EQUAL(nBeforeEN+pTalentInfo->m_nMinDamage, nAfterEN);
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_TALENT_DRAIN, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 0));
			CHECK(pTalentInfo->m_Drain.m_nType == pLink->GetParam<unsigned char>(0, 1));
			CHECK_EQUAL(pTalentInfo->m_nMinDamage, pLink->GetParam<int>(0, 2));
		}

		TEST_FIXTURE(FTalentDrain, STA)
		{
			pTalentInfo->m_Drain.m_nType = TALENT_DRAIN_SP;
			pPlayer->SetSTA(1);

			int nBeforeSTA = pPlayer->GetSTA();
			MockLink* pLink = NewLink(pPlayer);
			pLink->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
			pLink->AddIgnoreID(MC_CHAR_DEL_ENEMYNPC);
			pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
			pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
			pNPC->doTryHit(pPlayer, pTalentInfo);
			int nAfterSTA = pPlayer->GetSTA();

			CHECK_EQUAL(nBeforeSTA+pTalentInfo->m_nMinDamage, nAfterSTA);
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_ACTION_TALENT_DRAIN, pLink->GetCommand(0).GetID());
			CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 0));
			CHECK(pTalentInfo->m_Drain.m_nType == pLink->GetParam<unsigned char>(0, 1));
			CHECK_EQUAL(pTalentInfo->m_nMinDamage, pLink->GetParam<int>(0, 2));
		}
	}
		
	TEST_FIXTURE(FTalentDrain, DrainRate)
	{
		pTalentInfo->m_Drain.m_nType = TALENT_DRAIN_HP;
		pTalentInfo->m_Drain.m_fRate = 0.5f;
		pPlayer->SetHP(1);

		int nBeforeHP = pPlayer->GetHP();
		MockLink* pLink = NewLink(pPlayer);
		pLink->AddIgnoreID(MC_CHAR_ADD_ENEMYNPC);
		pLink->AddIgnoreID(MC_CHAR_DEL_ENEMYNPC);
		pLink->AddIgnoreID(MC_NPCINTERACTION_ICON);
		pLink->AddIgnoreID(MC_CHAR_ENEMY_INFO);
		pNPC->doTryHit(pPlayer, pTalentInfo);
		int nAfterHP = pPlayer->GetHP();

		CHECK_EQUAL(nBeforeHP+pTalentInfo->m_nMinDamage*pTalentInfo->m_Drain.m_fRate, nAfterHP);
		CHECK_EQUAL(1, pLink->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_TALENT_DRAIN, pLink->GetCommand(0).GetID());
		CHECK_EQUAL(pTalentInfo->m_nID, pLink->GetParam<int>(0, 0));
		CHECK(pTalentInfo->m_Drain.m_nType == pLink->GetParam<unsigned char>(0, 1));
		CHECK_EQUAL(pTalentInfo->m_nMinDamage*pTalentInfo->m_Drain.m_fRate, pLink->GetParam<int>(0, 2));
	}
}