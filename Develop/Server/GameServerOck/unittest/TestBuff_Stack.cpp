#include "stdafx.h"
#include "GTestForward.h"
#include "GModuleBuff.h"
#include "FBasePlayer.h"
#include "CSTalentInfoEnum.h"
#include "GBuff.h"

SUITE(Buff_Stack)
{
	struct FBuff : public FBasePlayer
	{
		FBuff()
		{
			m_pField = GUTHelper_Field::DefaultMockField();
			m_pPlayer = NewEntityPlayer(m_pField, vec3(100,100,0));
			swprintf_s(m_pPlayer->GetPlayerInfo()->szName, L"Tester");
			m_pLinkTester = test.network.NewLink(m_pPlayer);

			pmBuff =  m_pPlayer->GetModuleBuff();

			SetIgnoreCommonCommand(m_pLinkTester);
		}

		~FBuff()
		{
			test.network.ClearLinks();
			m_pField->Destroy();
		}

		void BeginCommandRecord()
		{
			m_pLinkTester->ResetCommands();
		}

		void SetIgnoreCommonCommand(MockLink* pLink)
		{
			pLink->AddIgnoreID(MC_ACTION_USE_TALENT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT);
			pLink->AddIgnoreID(MC_ACTION_TALENT_HIT_NO_MF);
		}

		GBuffInfo* NewStackBuffInfo(BUFF_STACK_TYPE nStackType)
		{
			return NewStackBuffInfo(INVALID_BUFF_ID, nStackType);
		}

		GBuffInfo* NewStackBuffInfo(int nBuffID, BUFF_STACK_TYPE nStackType)
		{
			GBuffInfo* pInfo=test.buff.NewBuffInfo(nBuffID);
			pInfo->m_nStackType = nStackType;
			return pInfo;
		}

		MockField*		m_pField;
		GEntityPlayer*	m_pPlayer;
		GModuleBuff*	pmBuff;
		MockLink*		m_pLinkTester;

		DECLWRAPPER_FieldInfoMgr;
		DECLWRAPPER_FieldMgr;
		DECLWRAPPER_BuffMgr;
	};

	TEST_FIXTURE(FBuff, IsStackable)
	{
		int nOhterBuffID = 321;
		CHECK_EQUAL(false, pmBuff->IsStackable(nOhterBuffID)); // 없는 버프
		
		// BUFFSTACK_NONE
		{
			int nBuffID = 123;
			GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_NONE);
			test.buff.GainBuffSelf(m_pPlayer, pInfo);
			CHECK_EQUAL(false, pmBuff->IsStackable(nOhterBuffID));
			CHECK_EQUAL(false, pmBuff->IsStackable(nBuffID));
		}

		// BUFFSTACK_STACK
		{
			int nBuffID = 124;
			GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK);
			test.buff.GainBuffSelf(m_pPlayer, pInfo);
			CHECK_EQUAL(false, pmBuff->IsStackable(nOhterBuffID));
			CHECK_EQUAL(true, pmBuff->IsStackable(nBuffID));
		}
		
		// BUFFSTACK_STACK_UPDATE
		{
			int nBuffID = 125;
			GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
			test.buff.GainBuffSelf(m_pPlayer, pInfo);
			CHECK_EQUAL(false, pmBuff->IsStackable(nOhterBuffID));
			CHECK_EQUAL(true, pmBuff->IsStackable(nBuffID));
		}
	}

	TEST_FIXTURE(FBuff, MaxStackCount)
	{
		int nBuffID = 123;
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
		pInfo->m_nStackMaxCount = 2;
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		CHECK_EQUAL(2, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		CHECK_EQUAL(2, pBuff->GetStackCount());
	}
	
	TEST_FIXTURE(FBuff, LifeCycle)
	{
		int nBuffID = 123;
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		CHECK_EQUAL(2, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo);
		CHECK_EQUAL(3, pBuff->GetStackCount());
		pBuff->Cancel();
		CHECK_EQUAL(2, pBuff->GetStackCount());
		pBuff->Cancel();
		CHECK_EQUAL(1, pBuff->GetStackCount());
		pBuff->Cancel();
		pmBuff->Update(0.1f);
		CHECK(pmBuff->FindBuffByID(nBuffID) == NULL);
	}

	TEST_FIXTURE(FBuff, LifeCycle_StackUpdate)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		CHECK_EQUAL(2, pBuff->GetStackCount());
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(1.0f);
		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(1.0f);
		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(DURATION_TIME);
		pmBuff->Update(0.1f);
		CHECK(pmBuff->FindBuffByID(nBuffID) == NULL);
	}

	TEST_FIXTURE(FBuff, LifeCycle_Stack)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		const float PERIOD_TIME = 2.0f;
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK);
		pInfo->m_fPeriod = PERIOD_TIME;
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(PERIOD_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(PERIOD_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(PERIOD_TIME);
		CHECK_EQUAL(2, pBuff->GetStackCount());
		pmBuff->Update(PERIOD_TIME);
		CHECK_EQUAL(1, pBuff->GetStackCount());
		pmBuff->Update(PERIOD_TIME);
		pmBuff->Update(0.1f);
		CHECK(pmBuff->FindBuffByID(nBuffID) == NULL);
	}

	TEST_FIXTURE(FBuff, LifeCycle_PopRule)
	{
		// 스택 카운트가 최대수치에 도달했을 때 스택에 새로 추가되면 가장 적게 남은 시간부터 뺀다.
		int nBuffID = 123;
		const float LONG_DURATION_TIME = 100.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		const float SHORT_DURATION_TIME = 10.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		const float PERIOD_TIME = 2.0f;
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK);
		pInfo->m_fPeriod = PERIOD_TIME;
		pInfo->m_nStackMaxCount = 3; // 최대 스택갯수
		test.buff.GainBuffSelf(m_pPlayer, pInfo, LONG_DURATION_TIME);
		pmBuff->Update(PERIOD_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, SHORT_DURATION_TIME);
		pmBuff->Update(PERIOD_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, LONG_DURATION_TIME);
		pmBuff->Update(PERIOD_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, LONG_DURATION_TIME);

		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(SHORT_DURATION_TIME);
		CHECK_EQUAL(3, pBuff->GetStackCount());
		pmBuff->Update(LONG_DURATION_TIME);
		pmBuff->Update(0.1f);
		CHECK(pmBuff->FindBuffByID(nBuffID) == NULL);
	}

	TEST_FIXTURE(FBuff, StackedDamage)
	{
		const float DURATION_TIME = 6.0f;
		const int DAMAGE = 10;

		// 보정값이 있는 버프 생성
		GBuffInfo* pInfo=NewStackBuffInfo(BUFFSTACK_STACK_UPDATE);
		pInfo->m_fPeriod = 2.0f;
		pInfo->m_nMinDamage = DAMAGE;
		pInfo->m_nMaxDamage = DAMAGE;

		// 플레이어 수치값 지정
		m_pPlayer->SetHP(200);

		// 버프 적용
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		// 버프 얻을 때 효과
		CHECK_EQUAL(m_pPlayer->GetHP(), 190);

		// 1차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 180);

		// 버프 중첩
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(2.0f);
		CHECK_EQUAL(m_pPlayer->GetHP(), 160);

		// 2차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 140);
	}

	TEST_FIXTURE(FBuff, StackedHeal)
	{
		const float DURATION_TIME = 6.0f;
		const int HEAL_AMOUNT = 10;

		// 보정값이 있는 버프 생성
		GBuffInfo* pInfo=NewStackBuffInfo(BUFFSTACK_STACK_UPDATE);
		pInfo->m_fPeriod = 2.0f;
		pInfo->m_nMinHeal = HEAL_AMOUNT;
		pInfo->m_nMaxHeal = HEAL_AMOUNT;

		// 플레이어 수치값 지정
		m_pPlayer->SetHP(200);

		// 버프 적용
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		// 버프 얻을 때 효과
		CHECK_EQUAL(m_pPlayer->GetHP(), 210);

		// 1차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 220);

		// 버프 중첩
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(2.0f);
		CHECK_EQUAL(m_pPlayer->GetHP(), 240);

		// 2차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 260);
	}

	TEST_FIXTURE(FBuff, StackedInstantModifier)
	{
		const float DURATION_TIME = 6.0f + 0.1f;
		const int HEAL_AMOUNT = 10;

		// 보정값이 있는 버프 생성
		GBuffInfo* pInfo=NewStackBuffInfo(BUFFSTACK_STACK);
		pInfo->m_fPeriod = 2.0f;
		pInfo->m_InstantModifier.nHP.nMod = 10;

		// 플레이어 수치값 지정
		m_pPlayer->SetHP(200);

		// 버프 적용
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		// 버프 얻을 때 효과
		CHECK_EQUAL(m_pPlayer->GetHP(), 210);

		// 1차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 220);

		// 버프 중첩
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(2.0f);
		CHECK_EQUAL(m_pPlayer->GetHP(), 240);

		// 2차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 260);

		// 버프 하나 만료되고 난 다음 효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 280);

		// 둘 다 만료
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetHP(), 290);
	}

	TEST_FIXTURE(FBuff, StackedPlayerModifier)
	{
		const float DURATION_TIME = 6.0f + 0.1f;
		const int HEAL_AMOUNT = 10;

		// 보정값이 있는 버프 생성
		GBuffInfo* pInfo=NewStackBuffInfo(BUFFSTACK_STACK);
		pInfo->m_fPeriod = 2.0f;
		pInfo->m_ActorModifier.nHPMax.nMod = 10;
		pInfo->m_PlayerModifier.nSTR.nMod = 10;

		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 800);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 100);

		// 버프 적용
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		// 버프 얻을 때 효과
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 810);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 110);

		// 1차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 820);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 120);

		// 버프 중첩
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		pmBuff->Update(2.0f);
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 840);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 140);

		// 2차 주기효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 860);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 160);

		// 버프 하나 만료되고 난 다음 효과
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 880);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 180);

		// 둘 다 만료
		pmBuff->Update(2.0f);	
		CHECK_EQUAL(m_pPlayer->GetMaxHP(), 800);
		CHECK_EQUAL(m_pPlayer->GetSTR(), 100);
	}

	TEST_FIXTURE(FBuff, StackPacketProtocol)
	{
		m_pLinkTester->AddIgnoreID(MC_BUFF_GAIN);
		m_pLinkTester->AddIgnoreID(MC_BUFF_LOST);

		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
				
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		
		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		// 패킷 체크
		{
			const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BUFF_STACK_INCREASE);
			TVALID(Command.GetParameterCount() >= 3);
			UIID nPacketActorUIID;
			int nPacketBuffID;
			float fPacketDuration;
			if (!Command.GetParameter(&nPacketActorUIID,	0, MPT_USHORT))	{ CHECK(false); return; }
			if (!Command.GetParameter(&nPacketBuffID,	1, MPT_INT))		{ CHECK(false); return; }
			if (!Command.GetParameter(&fPacketDuration,	2, MPT_FLOAT))		{ CHECK(false); return; }
			CHECK_EQUAL(nPacketActorUIID, m_pPlayer->GetUIID());
			CHECK_EQUAL(nPacketBuffID, nBuffID);
			CHECK_EQUAL(fPacketDuration, DURATION_TIME);
		}
		
		BeginCommandRecord();
		pBuff->Cancel();
		
		// 패킷 체크
		{
			const minet::MCommand& Command = m_pLinkTester->GetCommand(0);
			CHECK_EQUAL(Command.GetID(), MC_BUFF_STACK_DECREASE);
			TVALID(Command.GetParameterCount() >= 2);
			UIID nPacketActorUIID;
			int nPacketBuffID;
			if (!Command.GetParameter(&nPacketActorUIID,	0, MPT_USHORT))		{ CHECK(false); return; }
			if (!Command.GetParameter(&nPacketBuffID,		1, MPT_INT))		{ CHECK(false); return; }
			CHECK_EQUAL(nPacketActorUIID, m_pPlayer->GetUIID());
			CHECK_EQUAL(nPacketBuffID, nBuffID);
		}
	}

	TEST_FIXTURE(FBuff, StackPacket_Flow)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);

		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_GAIN, m_pLinkTester->GetCommand(0).GetID());

		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_STACK_INCREASE, m_pLinkTester->GetCommand(0).GetID());

		BeginCommandRecord();
		pBuff->Cancel();
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_STACK_DECREASE, m_pLinkTester->GetCommand(0).GetID());

		BeginCommandRecord();
		pBuff->Cancel();
		pmBuff->Update(0.1f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_LOST, m_pLinkTester->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FBuff, StackPacket_Lost)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);

		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);

		BeginCommandRecord();
		pmBuff->Update(DURATION_TIME);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_LOST, m_pLinkTester->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FBuff, SingleBuffPacket_Lost)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_NONE);

		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_GAIN, m_pLinkTester->GetCommand(0).GetID());

		BeginCommandRecord();
		pBuff->Cancel();
		pmBuff->Update(0.1f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_LOST, m_pLinkTester->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FBuff, SingleStackBuffPacket_Lost)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);

		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		GBuff* pBuff = pmBuff->FindBuffByID(nBuffID);
		TVALID(pBuff);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_GAIN, m_pLinkTester->GetCommand(0).GetID());

		BeginCommandRecord();
		pBuff->Cancel();
		pmBuff->Update(0.1f);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_LOST, m_pLinkTester->GetCommand(0).GetID());
	}

	TEST_FIXTURE(FBuff, StackBuffPacket_MaxCount)
	{
		int nBuffID = 123;
		const float DURATION_TIME = 6.0f - 0.1f; // 0.1 빼는 것은 INIFITY 타임과 중복되지 않기 위함 
		GBuffInfo* pInfo=NewStackBuffInfo(nBuffID, BUFFSTACK_STACK_UPDATE);
		pInfo->m_nStackMaxCount = 3;

		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		BeginCommandRecord();
		test.buff.GainBuffSelf(m_pPlayer, pInfo, DURATION_TIME);
		CHECK_EQUAL(1, m_pLinkTester->GetCommandCount());
		CHECK_EQUAL(MC_BUFF_STACK_INCREASE, m_pLinkTester->GetCommand(0).GetID());
	}
}