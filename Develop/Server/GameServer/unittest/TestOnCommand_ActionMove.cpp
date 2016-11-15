#include "stdafx.h"
#include "FBaseGame.h"
#include "GTestForward.h"
#include "GTalent.h"
#include "GModuleCombat.h"

SUITE(OnCommand_ActionMove)
{
	class FOnCommand_ActionMove : public FBaseGame
	{
	public:
		FOnCommand_ActionMove() 
		{
			InitPlayerAndLink(&m_pPlayer1, &m_pLink1);
			InitPlayerAndLink(&m_pPlayer2, &m_pLink2);
		}
		virtual ~FOnCommand_ActionMove()
		{

		}
		GEntityPlayer* m_pPlayer1;
		GEntityPlayer* m_pPlayer2;
		MockLink*		m_pLink1;
		MockLink*		m_pLink2;
	};

	TEST_FIXTURE(FOnCommand_ActionMove, Test_OnCommand_MC_ACTION_MOVE_REQ)
	{
		UIID nPlayerUIID1 = m_pPlayer1->GetUIID();
		TD_PC_MOVE tdPcMove;
		
		tdPcMove.nFlags = MOVEMENTFLAG_FORWARD;
		tdPcMove.vTarPos = vec3(0.0f, 100.0f, 0.0f);
		tdPcMove.svDir.SetX(0.5f);
		tdPcMove.svDir.SetY(0.5f);

		m_pLink1->OnRecv(MC_ACTION_MOVE_REQ, 
							1, 
							NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));


		CHECK_EQUAL(m_pPlayer1->GetPos(), tdPcMove.vTarPos);
		CHECK_CLOSE(m_pPlayer1->GetDir().x, tdPcMove.svDir.X(), 0.001f);
		CHECK_CLOSE(m_pPlayer1->GetDir().y, tdPcMove.svDir.Y(), 0.001f);



		// 주위 플레이어에게 보낸 커맨드 체크
		CHECK_EQUAL(1, m_pLink2->GetCommandCount());
		CHECK_EQUAL(MC_ACTION_MOVE, m_pLink2->GetCommandID(0));
		CHECK_EQUAL(uint16(nPlayerUIID1), m_pLink2->GetParam<uint16>(0, 0));

		MCommandParameter* pParam = m_pLink2->GetCommand(0).GetParameter(1);
		TD_PC_MOVE* pSendTDPcMove = (TD_PC_MOVE*)pParam->GetPointer();
		
		CHECK_EQUAL(tdPcMove.nFlags, pSendTDPcMove->nFlags);
		CHECK_EQUAL(tdPcMove.vTarPos.x, pSendTDPcMove->vTarPos.x);
		CHECK_EQUAL(tdPcMove.vTarPos.y, pSendTDPcMove->vTarPos.y);
		CHECK_EQUAL(tdPcMove.vTarPos.z, pSendTDPcMove->vTarPos.z);

		CHECK_EQUAL(tdPcMove.svDir.X(), pSendTDPcMove->svDir.X());
		CHECK_EQUAL(tdPcMove.svDir.Y(), pSendTDPcMove->svDir.Y());
	}

	TEST_FIXTURE(FOnCommand_ActionMove, MC_ACTION_MOVE_REQ__CancelCurTalent)
	{
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(1);
		pAtkTalent->m_fCastingTime = 5.0f;
		pAtkTalent->m_fDurationTime = 10.0f;
		pAtkTalent->Cooking();

		// 탤런트 사용
		CHECK(!m_pPlayer1->GetCurrentTalent());
		test.talent.UseTalent(m_pPlayer1, pAtkTalent, m_pPlayer2->GetUID());
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(false, m_pPlayer1->GetCurrentTalent()->IsExpired());

		// 이동
		UIID nPlayerUIID1 = m_pPlayer1->GetUIID();
		TD_PC_MOVE tdPcMove;
		tdPcMove.nFlags = MOVEMENTFLAG_FORWARD;
		tdPcMove.vTarPos = vec3(0.0f, 100.0f, 0.0f);
		tdPcMove.svDir.SetX(0.5f);
		tdPcMove.svDir.SetY(0.5f);
		m_pLink1->OnRecv(MC_ACTION_MOVE_REQ, 
			1, 
			NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));

		// 탤런트 취소됨
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(true, m_pPlayer1->GetCurrentTalent()->IsExpired());
	}

	TEST_FIXTURE(FOnCommand_ActionMove, MC_ACTION_MOVE_REQ__MovingTalent)
	{
		GTalentInfo* pAtkTalent = test.talent.MakeDamageTalent(1);
		pAtkTalent->m_fCastingTime = 5.0f;
		pAtkTalent->m_fDurationTime = 10.0f;
		pAtkTalent->m_nMovableType = MUT_CASTING;
		pAtkTalent->Cooking();

		// 탤런트 사용
		CHECK(!m_pPlayer1->GetCurrentTalent());
		test.talent.UseTalent(m_pPlayer1, pAtkTalent, m_pPlayer2->GetUID());
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(false, m_pPlayer1->GetCurrentTalent()->IsExpired());

		// 이동
		UIID nPlayerUIID1 = m_pPlayer1->GetUIID();
		TD_PC_MOVE tdPcMove;
		tdPcMove.nFlags = MOVEMENTFLAG_FORWARD;
		tdPcMove.vTarPos = vec3(0.0f, 100.0f, 0.0f);
		tdPcMove.svDir.SetX(0.5f);
		tdPcMove.svDir.SetY(0.5f);
		m_pLink1->OnRecv(MC_ACTION_MOVE_REQ, 
			1, 
			NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));

		// 탤런트 취소안됨
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(false, m_pPlayer1->GetCurrentTalent()->IsExpired());

		// 두번째 케이스
		pAtkTalent->m_nMovableType = MUT_ALL;
		pAtkTalent->Cooking();

		// 탤런트 사용
		test.talent.UseTalent(m_pPlayer1, pAtkTalent, m_pPlayer2->GetUID());
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(false, m_pPlayer1->GetCurrentTalent()->IsExpired());

		// 이동
		m_pLink1->OnRecv(MC_ACTION_MOVE_REQ, 
			1, 
			NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));

		// 탤런트 취소안됨
		TVALID(m_pPlayer1->GetCurrentTalent());
		CHECK_EQUAL(false, m_pPlayer1->GetCurrentTalent()->IsExpired());
	}
}