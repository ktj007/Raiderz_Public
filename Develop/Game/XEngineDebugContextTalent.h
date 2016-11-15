#pragma once

#include "XEngineDebugContext.h"

class XEngineDebugContextTalent : public XEngineDebugContext
{
public:
	XEngineDebugContextTalent()
		: XEngineDebugContext(XEngineDebugContext::DCT_MY_CHARACTER_WORLD)
	{
		m_vStartPosNet.Set(500,500,0);
		m_vStartDirNet.Set(1,1,0);
		m_vStartDirNet.Normalize();

		m_vStartPosMy = m_vStartPosNet + (300.f * m_vStartDirNet);
		m_vStartDirMy = -m_vStartDirNet;
	}

	virtual ~XEngineDebugContextTalent(){}

	virtual void OnUpdate(float fDelta)
	{
	}

	virtual void OnCmd(const std::vector< std::wstring >& _refConsoleCmd)
	{
		_ASSERT( _refConsoleCmd.empty() == false );

		if (_refConsoleCmd[0] == L"init")
		{
			InitHitDebuggingWorld();
		}
		else
		{
			if (false == m_bContextUsable)
			{
				m_pEngineDebugger->PrintToConsole(L"카메라를 테스트 할수 있는 환경 아님 - 'ed s init' 요망");
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 서브 커맨드
			//
			if(_refConsoleCmd[0] == L"a")
			{
				// NPC 화원의 주인(121019) 으로 테슽 하셔야 합니다.
				minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
				XNonPlayer* pOgre = gg.omgr->FindNPC(m_NPCID);
				pOgre->ChangeMode(NPC_MODE_1);


				pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
					NEW_USHORT( m_NPCUIID ),
					NEW_INT( 212101929 ),
					NEW_SVEC(pOgre->GetDirection()),
					NEW_VEC(pOgre->GetPosition())
					);
				pNetAgent->OnRecv( MC_ACTION_ACT_TALENT, 3,
					NEW_USHORT( m_NPCUIID ),
					NEW_INT( 212101929 ),
					NEW_SVEC(pOgre->GetDirection())
					);

				return;
			}
		}
	}

	void InitHitDebuggingWorld()
	{
		// 월드
		XWorld* pWorld = gg.currentgamestate->GetWorld();

		XWorldInitParam worldInitParam;
		worldInitParam.SetZone(L"BirdTown", true, false);

		pWorld->Init(worldInitParam);

		m_NPCID = global.system->GenerateLocalUID();
		m_NPCUIID = XUnitTestUtil::NewUIID();

		// Net Player

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_NPCID;
		td_update_cache_npc.nUIID = m_NPCUIID;
		td_update_cache_npc.nNPCID = 121019;
		td_update_cache_npc.vPos = m_vStartPosNet;
		td_update_cache_npc.svDir = m_vStartDirNet;
		td_update_cache_npc.nTalentID = 0;
		td_update_cache_npc.uidTarget = MUID::ZERO;

		gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);
		
		// 사용가능하게 됨
		m_bContextUsable = true;

		// 카메라
		rs3::RCameraSceneNode* pCamera = global.smgr->GetPrimaryCamera();
		if (pCamera != NULL)
		{
			pCamera->SetTransform( RVector(1051.1499, 1928.9570, 577.73804), RVector(-0.42497706, -0.85046417, -0.31000835) );
			pCamera->UpdateTransform();
		}
	}

	MUID m_MyPlayerID;
	MUID m_NPCID;

	UIID m_MyPlayerUIID;
	UIID m_NPCUIID;

	RVector m_vStartPosMy;
	RVector m_vStartDirMy;

	RVector m_vStartPosNet;
	RVector m_vStartDirNet;


	bool m_bContextUsable;
};
