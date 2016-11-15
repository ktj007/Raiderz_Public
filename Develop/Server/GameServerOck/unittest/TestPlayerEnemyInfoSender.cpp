#include "stdafx.h"
#include "GEntityPlayer.h"
#include "GUTHelper.h"
#include "MockField.h"
#include "CCommandTable.h"
#include "GFieldInfoMgr.h"
#include "GTestWrapper.h"
#include "GNPCMgr.h"
#include "GTalentInfoMgr.h"
#include "MockLink.h"
#include "GFieldMgr.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GHateTable.h"
#include "FBaseMockLink.h"
#include "GUTHelper_Field.h"
#include "GTestForward.h"
#include "GDuelMgr.h"
#include "GDuel.h"

SUITE(PlayerEnemyInfoSender)
{
	const float UPDATE_TICK = 1.0f;

	struct FPlayerEnemyInfoSender : public FBaseMockLink
	{
		FPlayerEnemyInfoSender() 
		{
			pField = GUTHelper_Field::DefaultMockField();
			pPlayer = GUTHelper::NewEntityPlayer(pField, vec3(1000,1000,0));
			pLink1 = GUTHelper::NewLink(pPlayer, true);
			pLink1->AddAllowedID(MC_CHAR_ENEMY_INFO);
			pLink1->AddAllowedID(MC_CHAR_ENEMY_INFO_CLEAR);

			// 결투깃발 NPCID 등록
			GNPCInfo* pinfoNPC = new GNPCInfo();
			pinfoNPC->nID = DUEL_FLAG_NPCID;
			pinfoNPC->nMaxHP = 50;
			gmgr.pNPCInfoMgr->Insert(pinfoNPC);
		}
		~FPlayerEnemyInfoSender()
		{
			pField->Destroy();
		}

		DECLWRAPPER_Field;
		DECLWRAPPER_NPCInfoMgr;
		DECLWRAPPER_TalentMgr;
		GUTHelper helper;
		MockField* pField;
		GEntityPlayer* pPlayer;
		MockLink* pLink1;
	};

	TEST_FIXTURE(FPlayerEnemyInfoSender, Basic)
	{		
		GEntityNPC* pNPC1 = helper.NewEntityNPC(pField, vec3(1000.0f, 1000.0f, 0.0f));
		GEntityNPC* pNPC2 = helper.NewEntityNPC(pField, vec3(1000.0f, 1000.0f, 0.0f));	
		GTalentInfo* pTalentInfo = helper.NewTalentInfo();

		vector<TD_ENEMY_INFO> vecEnemyInfoParams;

		// npc1을 공격 ---
		pPlayer->OnHitEnemy(CTR_NORMAL, pNPC1, pTalentInfo);
		pNPC1->GetHateTable().AddPoint_GainDamage(pPlayer, 1);
		pPlayer->Update(UPDATE_TICK);		
		CHECK_EQUAL(1, pLink1->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(0).GetID());
		pLink1->GetCommand(0).GetBlob(vecEnemyInfoParams, 0);
		CHECK_EQUAL(1, vecEnemyInfoParams.size());
		CHECK_EQUAL(pNPC1->GetUIID(), vecEnemyInfoParams[0].nUIID);
		CHECK_EQUAL(100, vecEnemyInfoParams[0].nHPPercent);		


		// 시간이 지나도 보낸 것과 변하지 않으면 보내지 않음
		pPlayer->Update(UPDATE_TICK);
		pPlayer->Update(UPDATE_TICK);
		CHECK_EQUAL(pLink1->GetCommandCount(), 1);

		
		//// hp가 절반으로 줄어서 새로 보내준다.
		pNPC1->SetHP(pNPC1->GetHP() / 2);		
		pPlayer->Update(UPDATE_TICK);
		CHECK_EQUAL(2, pLink1->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(1).GetID());
		vecEnemyInfoParams.clear();
		pLink1->GetCommand(1).GetBlob(vecEnemyInfoParams, 0);
		CHECK_EQUAL(1, vecEnemyInfoParams.size());
		CHECK_EQUAL(pNPC1->GetUIID(), vecEnemyInfoParams[0].nUIID);
		CHECK_EQUAL(50, vecEnemyInfoParams[0].nHPPercent);		


		// npc 2를 공격 ----
		pPlayer->OnHitEnemy(CTR_NORMAL, pNPC2, pTalentInfo);
		pNPC2->GetHateTable().AddPoint_GainDamage(pPlayer, 1);
		pPlayer->Update(UPDATE_TICK);
		CHECK_EQUAL(3, pLink1->GetCommandCount());
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(2).GetID());
		vecEnemyInfoParams.clear();
		pLink1->GetCommand(2).GetBlob(vecEnemyInfoParams, 0);
		CHECK_EQUAL(2, vecEnemyInfoParams.size());
		CHECK_EQUAL(pNPC1->GetUIID(), vecEnemyInfoParams[0].nUIID);
		CHECK_EQUAL(50, vecEnemyInfoParams[0].nHPPercent);
		CHECK_EQUAL(pNPC2->GetUIID(), vecEnemyInfoParams[1].nUIID);
		CHECK_EQUAL(100, vecEnemyInfoParams[1].nHPPercent);

		
		// npc 2가 더이상 플레이어를 적으로 여기지 않음
		pLink1->ResetCommands();
		pNPC1->GetHateTable().Reset();
		pNPC2->GetHateTable().Reset();
		pPlayer->Update(UPDATE_TICK);
		// 적이 하나 줄었을때 정보 갱신
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(0).GetID());
		// 적이 모두 없어졌을때 정보 갱신
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO_CLEAR, pLink1->GetCommand(1).GetID());
	}

	TEST_FIXTURE(FPlayerEnemyInfoSender, ForPVP)
	{		
		GEntityPlayer* pEnemy = GUTHelper::NewEntityPlayer(pField, vec3(1000,1000,0));		

		// 플레이어가 공격
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(10);
		test.talent.UseTalent(pPlayer, pAtkTalent, pEnemy->GetUID());
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(0).GetID()); // 피격 이벤트

		// 전투 종료
		pPlayer->Update(GConst::PVP_DURATIONTIME);
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(1).GetID()); // 업데이트로 갱신된 HP때문에 피격 이벤트
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO_CLEAR, pLink1->GetCommand(2).GetID()); // 전투 종료
	}

	TEST_FIXTURE(FPlayerEnemyInfoSender, ForDuel)
	{		
		GEntityPlayer* pEnemy = GUTHelper::NewEntityPlayer(pField, vec3(1000,1000,0));		

		GDuelMgr duel_mgr(pField);
		duel_mgr.DuelRequest(pPlayer, pEnemy->GetUID());
		duel_mgr.DuelQuestionRespond(pPlayer->GetDuel(), true);
		CHECK_EQUAL(true, pPlayer->HasDuel());
		CHECK_EQUAL(true, pEnemy->HasDuel());
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(0).GetID()); // 피격 이벤트
		for (int i=0; i<DUEL_READY_COUNT; i++)
		{
			duel_mgr.Update(1.0f);
		}
		// 플레이어가 공격
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(10000); // 한방에 죽일 수 있는 탤런트
		test.talent.UseTalent(pPlayer, pAtkTalent, pEnemy->GetUID());
		duel_mgr.Update(0.0f);
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO, pLink1->GetCommand(1).GetID()); // 피격 이벤트
		CHECK_EQUAL(MC_CHAR_ENEMY_INFO_CLEAR, pLink1->GetCommand(2).GetID()); // 전투 종료
		CHECK_EQUAL(false, pPlayer->HasDuel());
		CHECK_EQUAL(false, pEnemy->HasDuel());
	}
}