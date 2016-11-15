#include "stdafx.h"
#include "GUnitTestRunner.h"
#include "FBaseGame.h"
#include "GSharedFieldMaster.h"
#include "GCollisionTree.h"
#include "GFieldFactory.h"
#include "GTestForward.h"
#include "GBuffEntity.h"

SUITE(Cmd_ActionMove)
{
	class FOnCommand_ActionMove : public FBaseGame
	{
	public:
		FOnCommand_ActionMove() 
		{
		}
		virtual ~FOnCommand_ActionMove()
		{

		}
	};

	TEST4(Cmd_Test)
	{
		// 플레이어 액터 생성
		// 로딩된 필드중에 랜덤한 필드 선택
		// 같은 필드에 인근의 위치로 랜덤하게 이동
		for (int i=0; i<1000; ++i)
		{
			AutoTestCaseTimer(AutoTestCaseTimer::SFormat(L"%d", i));
			int a=8;
			int b=11;
			int c=a+b;
			CHECK(true);		
		}
	}

	TEST4(Cmd_Test2)
	{
		CHECK(true);
	}

	TEST4(RandomMoveReq)
	{
		const float fRandomMoveGap = 200.0f;

		GFieldMgr::SHARED_FIELD_MASTER_MAP::iterator itr = gmgr.pFieldMgr->GetSharedFieldMap().begin();
		for (; itr != gmgr.pFieldMgr->GetSharedFieldMap().end(); ++itr)
		{
			for (int i=0; i<1000; ++i)
			{
				GSharedField* pSharedField = itr->second->GetChannel(1);
				const GFieldInfo* pFieldInfo = pSharedField->GetInfo();
				vec3 vStartPos(
					GMath::RandomNumber(pFieldInfo->MinX()+fRandomMoveGap, pFieldInfo->MaxX()-fRandomMoveGap),
					GMath::RandomNumber(pFieldInfo->MinY()+fRandomMoveGap, pFieldInfo->MaxY()-fRandomMoveGap),
					const_cast<GFieldInfo*>(pFieldInfo)->GetCollisionTree()->GetBoundingBox().maxz);
				//vec3 vStartPos(
				//	26437.7, 35646.8, -1.0);

				pSharedField->PickingZ(vStartPos, vStartPos);
				GEntityPlayer* pPlayer = GUTHelper::NewEntityPlayer(pSharedField, vStartPos);
				DCHECK(pPlayer);
				MockLink* m_pLink = GUTHelper::NewLink(pPlayer);

				// 이동 패킷
				TD_PC_MOVE tdPcMove;
				tdPcMove.nFlags = MOVEMENTFLAG_FORWARD;
				tdPcMove.vTarPos = vec3(
					vStartPos.x + GMath::RandomNumber(-fRandomMoveGap, fRandomMoveGap), 
					vStartPos.y + GMath::RandomNumber(-fRandomMoveGap, fRandomMoveGap), 
					vStartPos.z - 100.0f
					);
				//tdPcMove.vTarPos = vec3(
				//	26547.3, 35583.8, -101.0
				//	);
				tdPcMove.svDir.SetX(0.5f);
				tdPcMove.svDir.SetY(0.5f);

				{
					AutoTestCaseTimer(AutoTestCaseTimer::SFormat(L"count: %d, field: %d, start: %.1f %.1f %.1f, target: %.1f %.1f %.1f", 
						i, 
						pFieldInfo->m_nFieldID, 
						vStartPos.x, vStartPos.y, vStartPos.z, 
						tdPcMove.vTarPos.x, tdPcMove.vTarPos.y, tdPcMove.vTarPos.z
						));

					m_pLink->OnRecv(MC_ACTION_MOVE_REQ, 
						1, 
						NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));
				}
				
				pPlayer->DeleteMe();
				pSharedField->Update(0.0f);
			}
		}
	}

	//TEST_FIXTURE(FOnCommand_ActionMove, Warp)
	//{
	//	GUTHelper		m_Helper;
	//	GNPCInfo* pNPCInfo = m_Helper.NewNPCInfo();
	//	m_pPlayer->SetPos(vec3(0,0,0));

	//	int nBuffID = 123;
	//	GBuffInfo* pBuff=test.buff.NewBuffInfo(nBuffID);

	//	vec3 vSpawnAwayPoint = vec3(10000,10000,0);
	//	GTalentInfo* pTalentMagicArea = test.talent.NewMagicAreaTalentInfo();
	//	pTalentMagicArea->m_fDurationTime = 100.0f;
	//	pTalentMagicArea->m_EffectInfo.m_fRadius = 1000.0f;

	//	for (int i=0; i<100; ++i)
	//	{
	//		NewEntityPlayer(m_pField, vSpawnAwayPoint);
	//		m_pField->SpawnTestNPC(pNPCInfo, vSpawnAwayPoint); // 멀리 떨어진 곳에 스폰
	//		GBuffEntity::Spawn(m_pPlayer, vSpawnAwayPoint, nBuffID);
	//	}

	//	for (int i=0; i<100; ++i)
	//	{
	//		AutoTestCaseTimer("sector_move_test");
	//		//m_pPlayer->SetPos(vSpawnAwayPoint);

	//		m_pPlayer->SetPos(vec3(0,0,0));

	//		TD_PC_MOVE tdPcMove;
	//		tdPcMove.nFlags = MOVEMENTFLAG_FORWARD;
	//		tdPcMove.vTarPos = vSpawnAwayPoint;
	//		tdPcMove.svDir.SetX(0.5f);
	//		tdPcMove.svDir.SetY(0.5f);

	//		m_pLink->OnRecv(MC_ACTION_MOVE_REQ, 
	//			1, 
	//			NEW_SINGLE_BLOB(&tdPcMove, sizeof(TD_PC_MOVE)));
	//	}
	//}
}