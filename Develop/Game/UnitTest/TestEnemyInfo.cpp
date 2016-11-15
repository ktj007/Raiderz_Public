#include "stdafx.h"
#include "TestModule.h"
#include "XNPCInfo.h"
#include "XNonPlayer.h"

SUITE(EnemyInfo)
{
	TEST_FIXTURE(SimpleTestFixture, TestPlayerEnemyInfo)
	{
		int nNPCID = 1;
		vec3 vPos = vec3(1000.0f, 1000.0f, 0.0f);
		MUID uidNPC = global.system->GenerateLocalUID();

		XNonPlayer* pNPC = m_Helper.InNPC(uidNPC, nNPCID, vPos);

		UIID nNPCUIID = pNPC->GetUIID();

		TD_ENEMY_INFO nodes[10];
		nodes[0].nUIID = nNPCUIID;
		nodes[0].nHPPercent = 100;
		nodes[0].nLevel= 10;

		int nNodeCount = 1;

		m_pNetAgent->OnRecv(MC_CHAR_ENEMY_INFO, 1, NEW_BLOB(nodes, sizeof(TD_ENEMY_INFO), nNodeCount));


		CHECK_EQUAL(gvar.Game.EnemyInfo.GetEnemyCount(), 1);

		if (gvar.Game.EnemyInfo.GetEnemyCount() == 1)
		{
			TD_ENEMY_INFO* ptd_enemy_info = gvar.Game.EnemyInfo.GetEnemyInfo(0);

			CHECK_EQUAL(ptd_enemy_info->nUIID		, nNPCUIID);
			CHECK_EQUAL(ptd_enemy_info->nHPPercent, 100);
			CHECK_EQUAL(ptd_enemy_info->nLevel	, 10);
		}

		// 스크립트로 날라가는 테이블 체크
		//MWLua::table t = gvar.Game.EnemyInfo.GetEnemyInfoTable(0);
		//CHECK_EQUAL(t.get<char*>("name"), "sample");
		//CHECK_EQUAL(t.get<int>("hp")	, 100);
		//CHECK_EQUAL(t.get<int>("grade")	, 3);
		//CHECK_EQUAL(t.get<int>("level")	, 10);

		// 커맨드 새로 받음 ----
		// 하나의 NPC UID 로 테스트;;;
		nNodeCount = 1;
		nodes[0].nUIID = nNPCUIID;
		nodes[0].nHPPercent = 50;
		nodes[0].nLevel = 5;

		//nodes[1].UID = uidNPC;
		//nodes[1].nHPPercent = 100;
		//nodes[1].nLevel = 10;

		m_pNetAgent->OnRecv(MC_CHAR_ENEMY_INFO, 1, NEW_BLOB(nodes, sizeof(TD_ENEMY_INFO), nNodeCount));

		CHECK_EQUAL(gvar.Game.EnemyInfo.GetEnemyCount(), nNodeCount);

		if (gvar.Game.EnemyInfo.GetEnemyCount() == nNodeCount)
		{
			TD_ENEMY_INFO* ptd_enemy_info = gvar.Game.EnemyInfo.GetEnemyInfo(0);
			CHECK_EQUAL(ptd_enemy_info->nUIID		, nNPCUIID);
			CHECK_EQUAL(ptd_enemy_info->nHPPercent, 50);
			CHECK_EQUAL(ptd_enemy_info->nLevel	, 5);

			//td_enemy_info = gvar.Game.EnemyInfo.GetEnemyInfo(1);
			//CHECK_EQUAL(td_enemy_info.UID		, uidNPC);
			//CHECK_EQUAL(td_enemy_info.nHPPercent, 100);
			//CHECK_EQUAL(td_enemy_info.nLevel	, 10);
		}

		// 스크립트로 날라가는 테이블 체크
		//t = gvar.Game.EnemyInfo.GetEnemyInfoTable(0);
		//CHECK_EQUAL(t.get<char*>("name")	, "sample");
		//CHECK_EQUAL(t.get<int>("hp")		, 50);
		//CHECK_EQUAL(t.get<int>("grade")	, 3);
		//CHECK_EQUAL(t.get<int>("level")	, 5);

		//t = gvar.Game.EnemyInfo.GetEnemyInfoTable(1);
		//CHECK_EQUAL(t.get<char*>("name")	, "sample");
		//CHECK_EQUAL(t.get<int>("hp")		, 100);
		//CHECK_EQUAL(t.get<int>("grade")	, 3);
		//CHECK_EQUAL(t.get<int>("level")	, 10);

		// 커맨드 새로 받음 ------
		m_pNetAgent->OnRecv(MC_CHAR_ENEMY_INFO_CLEAR, 0, NULL);
		CHECK_EQUAL(gvar.Game.EnemyInfo.GetEnemyCount() , 0);

		// 정리
		info.npc->Clear();
	}
}