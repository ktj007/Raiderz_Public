#include "stdafx.h"
#include "GTestForward.h"
#include "FBaseScriptGlue.h"
#include "MockField.h"
#include "GModuleAI.h"

const vec3 vCommonPos = vec3(1000.0f, 1000.0f, 0.0f);
const vec3 vCommonDir = vec3(0,1,0);

SUITE(TestScriptFunction_Actor)
{
	struct Fixture: public FBaseScriptGlue
	{
		Fixture()			{}
		virtual ~Fixture()	{}
	};

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetUID)
	{
		const char* pszTestFuncName = "_Test_Actor_GetUID";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetHP();\n"
			"end";


		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		{
			int ret;
			WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), &ret);
			CHECK_EQUAL(ret, pPlayer->GetHP()); 
		}

		{
			int ret;
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
			CHECK_EQUAL(ret, pNPC->GetHP()); 
		}
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetFieldID)
	{
		const char* pszTestFuncName = "_Test_Actor_GetID";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetFieldID();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		int ret;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, 100);	// 필드 ID

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetDir)
	{
		const char* pszTestFuncName = "_Test_Actor_GetDir";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetDir();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		vec3 ret;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret.IsEqual(pNPC->GetDir()), true); 

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetPos)
	{
		const char* pszTestFuncName = "_Test_Actor_GetPos";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetPos();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		vec3 ret;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret.IsEqual(pNPC->GetPos()), true); 

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetCurrentMotionfactor)
	{
		const char* pszTestFuncName = "_Test_GetCurrentMotionfactor";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetMotionFactor();\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		int ret;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &ret);
		CHECK_EQUAL(ret, MF_NONE); 

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_DistanceTo)
	{
		const char* pszTestFuncName = "_Test_Actor_DistanceTo";
		const char* pszScript = 
			"function %s(Actor, OtherActor)\n"
			"	return Actor:DistanceTo(OtherActor);\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
		vec3 vOtherPos = vCommonPos;
		vOtherPos.x += 1523.7f;
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo, vOtherPos, vCommonDir);

		float ret;
		WCALL2(WLUA, pszTestFuncName, pNPC1->GetDelegator(), pNPC2->GetDelegator(), &ret);
		CHECK_CLOSE(pNPC1->GetPos().DistanceTo(pNPC2->GetPos()), ret, 0.1f); 

		pNPC1->Destroy();
		pNPC2->Destroy();
		SAFE_DELETE(pNPC1);
		SAFE_DELETE(pNPC2);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_Warp)
	{
		const char* pszTestFuncName = "_Test_Actor_Warp";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	Actor:Warp(vec3(1000, 2000, 3000));\n"
			"end";

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		// 플레이어 워프
		{
			GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
			CHECK_EQUAL(pPlayer->GetPos().IsEqual(vCommonPos), true); 
			WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);
			gmgr.pFieldMgr->Update(0.1f);
			CHECK_EQUAL(pPlayer->GetPos().IsEqual(vec3(1000, 2000, 3000)), true); 
		}

		// 엔피씨 워프
		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			CHECK_EQUAL(pNPC->GetPos().IsEqual(vCommonPos), true); 
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
			pNPC->GetModuleAI()->Update(0.0f);	// 직접호출에서 Job으로 이동으로 인해 AI update후 적용
			gmgr.pFieldMgr->Update(0.1f);
			CHECK_EQUAL(pNPC->GetPos().IsEqual(vec3(1000, 2000, 3000)), true); 
			pNPC->Destroy();
			SAFE_DELETE(pNPC);
		}
	}

	TEST_FIXTURE(Fixture, TestGlue_ActorTypeCheck)
	{
		const char* pszTestFuncName = "_Test_ActorTypeCheck";
		const char* pszScript = 
			"function %s(NPC, Player)\n"
			"	if (NPC:IsNPC()) then TEST_VAR1 = 1; end\n"
			"	if (Player:IsPlayer()) then TEST_VAR2 = 2; end\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), pPlayer->GetDelegator(), WNULL); // open dynamic field group

		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR1"), 1);
		CHECK_EQUAL(WLUA->GetVar<int>("TEST_VAR2"), 2);
	}

	TEST_FIXTURE(Fixture, TestGlue_SetMFModValue)
	{
		const char* pszTestFuncName = "_Test_SetMFModValue";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	Actor:SetMFModValue(MF_KNOCKDOWN, 1.7)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		CHECK_EQUAL(pPlayer->GetChrStatus()->ActorModifier.MotionFactorValue[MF_KNOCKDOWN].fPer, 0.0f);
		WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);
		CHECK_EQUAL(pPlayer->GetChrStatus()->ActorModifier.MotionFactorValue[MF_KNOCKDOWN].fPer, 1.7f);
	}

	TEST_FIXTURE(Fixture, TestGlue_SetMFModWeight)
	{
		const char* pszTestFuncName = "_Test_SetMFModWeight";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	Actor:SetMFModWeight(MF_KNOCKDOWN, 1.7)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);

		CHECK_EQUAL(pPlayer->GetChrStatus()->ActorModifier.MotionFactorWeight[MF_KNOCKDOWN].fPer, 0.0f);
		WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);
		CHECK_EQUAL(pPlayer->GetChrStatus()->ActorModifier.MotionFactorWeight[MF_KNOCKDOWN].fPer, 1.7f);
	}

	TEST_FIXTURE(Fixture, Glue_CheckBuff)
	{
		const char* pszTestFuncName = "_Test_HasBuff";
		const char* pszScript = 
			"function %s(NPC, BuffID)\n"
			"	return NPC:CheckBuff(BuffID)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		int nBuffID = 777;
		bool bRet=0;
		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), nBuffID, &bRet);
		CHECK_EQUAL(false, bRet);	// 버프 안걸림

		GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);
		test.buff.GainBuffSelf(pNPC, pBuff);

		WCALL2(WLUA, pszTestFuncName, pNPC->GetDelegator(), nBuffID, &bRet);
		CHECK_EQUAL(true, bRet);	// 버프 걸림

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, Glue_GetDebuffCount)
	{
		const char* pszTestFuncName = "_Test_GetDebuffCount";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GetDebuffCount()\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		int nRet=0;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(0, nRet);

		GBuffInfo* pBuff=test.buff.NewBuffInfo();
		pBuff->m_nType = BUFT_DEBUFF;
		test.buff.GainBuffSelf(pNPC, pBuff);

		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &nRet);
		CHECK_EQUAL(1, nRet);

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, Glue_GainBuff)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GainBuff(777)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GBuffInfo* pBuff=test.buff.NewBuffInfo(777);
		pBuff->m_nType = BUFT_DEBUFF;
		pBuff->m_InstantModifier.nHP.nMod = -1;
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pBuff->m_fDuration = 10.0f;
		pBuff->m_fPeriod = 2.0f;

		pNPC->SetHP(100);

		bool bRet=false;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bRet);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(true, bRet);	// 버프 성공적으로 걸림
		CHECK_EQUAL(true, pNPC->HasBuff(777));	// 777 버프가 걸려있음

		// 주기적 효과 테스트
		m_pField->Update(0.0f);
		CHECK_EQUAL(99, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(98, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(97, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(96, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(95, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(94, pNPC->GetHP());
		m_pField->Update(2.0f);
		CHECK_EQUAL(94, pNPC->GetHP());	// no effect

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, Glue_GainBuffDetail)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:GainBuffDetail(777, 3, 1)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GBuffInfo* pBuff=test.buff.NewBuffInfo(777);
		pBuff->m_nType = BUFT_DEBUFF;
		pBuff->m_InstantModifier.nHP.nMod = -1;
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;
		pBuff->m_fDuration = 10.0f;
		pBuff->m_fPeriod = 2.0f;

		pNPC->SetHP(100);

		bool bRet=false;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bRet);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(true, bRet);	// 버프 성공적으로 걸림
		CHECK_EQUAL(true, pNPC->HasBuff(777));	// 777 버프가 걸려있음

		// 주기적 효과 테스트
		m_pField->Update(0.0f);
		CHECK_EQUAL(99, pNPC->GetHP());
		m_pField->Update(1.0f);
		CHECK_EQUAL(98, pNPC->GetHP());
		m_pField->Update(1.0f);
		CHECK_EQUAL(97, pNPC->GetHP());
		m_pField->Update(1.0f);
		CHECK_EQUAL(96, pNPC->GetHP());
		m_pField->Update(1.0f);
		CHECK_EQUAL(96, pNPC->GetHP());

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}
	

	TEST_FIXTURE(Fixture, Glue_RemoveBuff)
	{
		const char* pszTestFuncName = "_Test_RemoveBuff";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	return NPC:RemoveBuff(777)\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GBuffInfo* pBuff=test.buff.NewBuffInfo(777);
		pBuff->m_nType = BUFT_DEBUFF;
		pBuff->m_InstantModifier.nHP.nMod = -1;
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

		test.buff.GainBuffSelf(pNPC, pBuff);
		CHECK_EQUAL(true, pNPC->HasBuff(777));	// 777 버프가 걸려있음

		bool bRet=false;
		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &bRet);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(true, bRet);	// 버프 성공적으로 걸림
		CHECK_EQUAL(false, pNPC->HasBuff(777));	// 777 버프가 풀림

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, Glue_RemoveAllBuff)
	{
		const char* pszTestFuncName = "_Test_RemoveBuff";
		const char* pszScript = 
			"function %s(NPC)\n"
			"	NPC:RemoveAllBuff()\n"
			"end\n"
			;
		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);

		GBuffInfo* pBuff=test.buff.NewBuffInfo(777);
		pBuff->m_nType = BUFT_DEBUFF;
		pBuff->m_InstantModifier.nHP.nMod = -1;
		pBuff->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_SELF;
		pBuff->m_EffectInfo.m_nRelation = CSEffectInfo::RELATION_ALL;

		test.buff.GainBuffSelf(pNPC, pBuff);
		CHECK_EQUAL(true, pNPC->HasBuff(777));	// 777 버프가 걸려있음

		WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
		pNPC->GetModuleAI()->Update(0.0f);
		CHECK_EQUAL(false, pNPC->HasBuff(777));	// 777 버프가 풀림

		pNPC->Destroy();
		SAFE_DELETE(pNPC);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetField)
	{
		const char* pszTestFuncName = "_Test_Actor_GetField";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetField()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		GGlueField* pFiled;

		{
			GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField, vCommonPos);			
			WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), &pFiled);
			CHECK_EQUAL(pFiled, pPlayer->GetField()->GetDelegator());
			CHECK_EQUAL(pFiled, m_pField->GetDelegator());
			CHECK_EQUAL(pFiled->GetOwnerField(), pPlayer->GetField());
			CHECK_EQUAL(pFiled->GetOwnerField(), m_pField);
		}

		{
			GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo, vCommonPos, vCommonDir);
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), &pFiled);
			CHECK_EQUAL(pFiled, pNPC->GetField()->GetDelegator());
			CHECK_EQUAL(pFiled, m_pField->GetDelegator());
			CHECK_EQUAL(pFiled->GetOwnerField(), pNPC->GetField());
			CHECK_EQUAL(pFiled->GetOwnerField(), m_pField);

			pNPC->Destroy();
			SAFE_DELETE(pNPC);
		}
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetHP)
	{
		const char* pszTestFuncName = "_Test_Actor_GetHP";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetHP()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		
		GEntityActor* pActor;
		test.actor.Init_ForActor<GEntityActor>(m_pField, pActor);
		pActor->SetHP(100);
		
		int hp = 0;
		WCALL1(WLUA, pszTestFuncName, pActor->GetDelegator(), &hp);

		CHECK_EQUAL(hp, pActor->GetHP());
		CHECK_EQUAL(hp, 100);

		pActor->Destroy();
		SAFE_DELETE(pActor);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetEN)
	{
		const char* pszTestFuncName = "_Test_Actor_GetEN";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetEN()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityActor* pActor;
		test.actor.Init_ForActor<GEntityActor>(m_pField, pActor);
		pActor->SetEN(100);

		int en = 0;
		WCALL1(WLUA, pszTestFuncName, pActor->GetDelegator(), &en);

		CHECK_EQUAL(en, pActor->GetEN());
		CHECK_EQUAL(en, 100);

		pActor->Destroy();
		SAFE_DELETE(pActor);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetSTA)
	{
		const char* pszTestFuncName = "_Test_Actor_GetSTA";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:GetSTA()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityActor* pActor;
		test.actor.Init_ForActor<GEntityActor>(m_pField, pActor);
		pActor->SetSTA(100);

		int sta = 0;
		WCALL1(WLUA, pszTestFuncName, pActor->GetDelegator(), &sta);

		CHECK_EQUAL(sta, pActor->GetSTA());
		CHECK_EQUAL(sta, 100);

		pActor->Destroy();
		SAFE_DELETE(pActor);
	}

	TEST_FIXTURE(Fixture, Actor_Narration)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	Actor:Narration(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);		
		MockLink* pLink = NewLink(pPlayer);

		{
			WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);
			
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_NPC_NARRATION, pLink->GetCommand(0).GetID());
			CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 0));
			pLink->ResetCommands();
		}

		{			
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
			pNPC->GetModuleAI()->Update(0.0f);	// 큐에 있는 잡을 시작

			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_NPC_NARRATION, pLink->GetCommand(0).GetID());
			CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 0));
			pLink->ResetCommands();
		}
	}

	TEST_FIXTURE(Fixture, Actor_NarrationNow)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	Actor:NarrationNow(\"Test\");\n"
			"end";

		TVALID(m_Helper.RunLuaString(pszScript, pszTestFuncName));

		GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(m_pField);
		GEntityNPC* pNPC = m_pField->SpawnTestNPC(m_pNPCInfo);		
		MockLink* pLink = NewLink(pPlayer);

		{
			WCALL1(WLUA, pszTestFuncName, pPlayer->GetDelegator(), WNULL);			
			
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_NPC_NARRATION, pLink->GetCommand(0).GetID());
			CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 0));
			pLink->ResetCommands();
		}

		{			
			WCALL1(WLUA, pszTestFuncName, pNPC->GetDelegator(), WNULL);
			
			CHECK_EQUAL(1, pLink->GetCommandCount());
			CHECK_EQUAL(MC_NPC_NARRATION, pLink->GetCommand(0).GetID());
			CHECK(wstring(L"Test") == pLink->GetParam<const wchar_t*>(0, 0));
			pLink->ResetCommands();
		}
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_GetFowardDir)
	{
		const char* pszTestFuncName = "_Test_Actor_GetSTA";
		const char* pszScript = 
			"function %s(Actor, Target)\n"
			"	return Actor:GetFowardDir(Target)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }

		
		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);		
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo);		

		pNPC1->SetPos(vec3(0,0,0));
		pNPC2->SetPos(vec3(0,100,0));
		pNPC1->SetDir(vec3(0,-1,0));

		vec3 vRet;
		WCALL2(WLUA, pszTestFuncName, pNPC1->GetDelegator(), pNPC2->GetDelegator(), &vRet);

		CHECK_EQUAL(true, vRet.IsEqual(vec3(0,1,0)));
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_IsRear)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor, Target)\n"
			"	return Actor:IsRear(Target)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);		
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo);		

		pNPC1->SetDir(vec3(0,1,0));
		pNPC1->SetPos(vec3(0,100,0));
		pNPC2->SetPos(vec3(0,0,0));

		bool ret;
		WCALL2(WLUA, pszTestFuncName, pNPC2->GetDelegator(), pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(true, ret);

		pNPC2->SetPos(vec3(0,200,0));
		WCALL2(WLUA, pszTestFuncName, pNPC2->GetDelegator(), pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(false, ret);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_IsFront)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor, Target)\n"
			"	return Actor:IsFront(Target)\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);		
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo);		

		pNPC1->SetDir(vec3(0,1,0));
		pNPC1->SetPos(vec3(0,100,0));
		pNPC2->SetPos(vec3(0,0,0));

		bool ret;
		WCALL2(WLUA, pszTestFuncName, pNPC2->GetDelegator(), pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(false, ret);

		pNPC2->SetPos(vec3(0,200,0));
		WCALL2(WLUA, pszTestFuncName, pNPC2->GetDelegator(), pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(true, ret);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_IsDead)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:IsDead()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);		

		bool ret=false;
		WCALL1(WLUA, pszTestFuncName, pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(false, ret);

		pNPC1->doDie();

		WCALL1(WLUA, pszTestFuncName, pNPC1->GetDelegator(), &ret);
		CHECK_EQUAL(true, ret);
	}

	TEST_FIXTURE(Fixture, TestGlue_Actor_MakePeace)
	{
		const char* pszTestFuncName = "_Test";
		const char* pszScript = 
			"function %s(Actor)\n"
			"	return Actor:MakePeace()\n"
			"end\n"
			;

		if (!m_Helper.RunLuaString(pszScript, pszTestFuncName))	{ CHECK(FALSE && "WRunString Failed.");	 return; }


		GEntityNPC* pNPC1 = m_pField->SpawnTestNPC(m_pNPCInfo);		
		GEntityNPC* pNPC2 = m_pField->SpawnTestNPC(m_pNPCInfo);		

		CHECK_EQUAL(false, pNPC1->IsNowCombat());

		GTalentInfo* pAttackTalent = test.talent.MakeDamageTalent(10);
		test.talent.UseTalent(pNPC2, pAttackTalent, pNPC1->GetUID());

		CHECK_EQUAL(true, pNPC1->IsNowCombat());
				
		WCALL1(WLUA, pszTestFuncName, pNPC1->GetDelegator(), WNULL);
		
		CHECK_EQUAL(true, pNPC1->IsNowCombat());
		CHECK_EQUAL(true, pNPC1->IsNowReturn());
	}
}
