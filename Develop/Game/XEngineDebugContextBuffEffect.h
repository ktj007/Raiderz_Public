#pragma once

#include "XEngineDebugContext.h"
#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

#include "RDebugPassRenderer.h"
#include "XUnitTestUtil.h"

class XEngineDebugContextBuffEffect : public XEngineDebugContext
{
public:
	XEngineDebugContextBuffEffect()
		: XEngineDebugContext(XEngineDebugContext::DCT_FREE_WORLD)
	{
		m_bNeedToReset = false;

		m_vStartPos.Set(500,500,0);
		m_vStartDir.Set(1,1,0);
		m_vStartDir.Normalize();

		m_vStartPosPlayer = m_vStartPos + (300.f * m_vStartDir);
		m_vStartDirPlayer = -m_vStartDir;
	}

	virtual ~XEngineDebugContextBuffEffect(){}

	virtual void OnUpdate(float fDelta)
	{
	}

	void InitDynamicActorNodeWorld()
	{
		// 월드
		XWorld* pWorld = gg.currentgamestate->GetWorld();

		XWorldInitParam worldInitParam;
		worldInitParam.SetZone(L"BirdTown", true, false);

		pWorld->Init(worldInitParam);

		UIID nVerusUIID = XUnitTestUtil::NewUIID();
		m_VerusMUID = global.system->GenerateLocalUID();
		m_PlayerID = global.system->GenerateLocalUID();

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_VerusMUID;
		td_update_cache_npc.nUIID = nVerusUIID;
		td_update_cache_npc.nNPCID = 106001;
		td_update_cache_npc.vPos = m_vStartPos;
		td_update_cache_npc.svDir = m_vStartDir;

		gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);

		// 버프 생성
		minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
		XActor* pOgre = gg.omgr->FindActor(m_VerusMUID);
		pNetAgent->OnRecv( MC_BUFF_GAIN, 3,
			NEW_USHORT( nVerusUIID ),
			NEW_INT( 40050 ),
			NEW_FLOAT(10)
			);

		// Net Player
		TD_UPDATE_CACHE_PLAYER td_update_cache_player;
		td_update_cache_player.uid = m_PlayerID;
		td_update_cache_player.vPos = m_vStartPosPlayer;
		td_update_cache_player.svDir = m_vStartDirPlayer;
		td_update_cache_player.nStance = CS_NORMAL;
		td_update_cache_player.nStatusFlag = 0;
		wcsncpy_s(td_update_cache_player.szName, L"pok", _TRUNCATE);

		// feature
		td_update_cache_player.Feature.nSex = SEX_FEMALE;
		td_update_cache_player.Feature.nHair = 0;
		td_update_cache_player.Feature.nFace = 0;
		td_update_cache_player.Feature.nWeaponSet = 0;
		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
			td_update_cache_player.Feature.nItemID[i] = 0;
		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
			td_update_cache_player.Feature.nItemID_DyedColor[i] = 0;

		gg.currentgamestate->InPlayer(td_update_cache_player.uid, &td_update_cache_player);

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

	virtual void OnCmd(const std::vector< std::wstring >& _refConsoleCmd)
	{
		_ASSERT(_refConsoleCmd.empty() == false);
		if (_refConsoleCmd[0] == L"init")
		{
			InitDynamicActorNodeWorld();
		}
		else
		{
			if (false == m_bContextUsable)
			{
				m_pEngineDebugger->PrintToConsole(L"랙돌을 테스트 할수 있는 환경 아님 - 'ed r init' 요망");
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 서브 커맨드
			//
			if (_refConsoleCmd[0] == L"a")
			{
				//StartDynamicActorNode();
				return;
			}
		}
	}

	bool m_bNeedToReset;

	MUID m_VerusMUID;
	MUID m_PlayerID;

	int m_nTalentID;
	int m_nGripPhase;
	float m_fElapsed;

	RVector m_vStartPos;
	RVector m_vStartDir;

	RVector m_vStartPosPlayer;
	RVector m_vStartDirPlayer;
};
