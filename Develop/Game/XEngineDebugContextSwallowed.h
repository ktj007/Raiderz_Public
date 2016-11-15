#pragma once

#include "XEngineDebugContext.h"
#include "MPhysX4R3/MPxDynamicActorNodeTC.h"

#include "RDebugPassRenderer.h"

#include "XTalentHitParam.h"
#include "XNonPlayer.h"
#include "XModuleActorControl.h"
#include "XModuleNetControl.h"

class XEngineDebugContextSwallowed : public XEngineDebugContext
{
public:
	XEngineDebugContextSwallowed()
		: XEngineDebugContext(XEngineDebugContext::DCT_FREE_WORLD)
	{
		m_bNeedToReset = false;

		m_vStartPos.Set(500,500,0);
		m_vStartDir.Set(1,1,0);
		m_vStartDir.Normalize();

		m_vStartPosPlayer = m_vStartPos + (300.f * m_vStartDir);
		m_vStartDirPlayer = -m_vStartDir;
	}

	virtual ~XEngineDebugContextSwallowed(){}

	virtual void OnUpdate(float fDelta)
	{
	}

	void InitSwallowedSynch()
	{
		// 월드
		XWorld* pWorld = gg.currentgamestate->GetWorld();

		XWorldInitParam worldInitParam;
		worldInitParam.SetZone(L"BirdTown", true, false);

		pWorld->Init(worldInitParam);

		m_VerusMUID = global.system->GenerateLocalUID();
		m_PlayerID = global.system->GenerateLocalUID();
		m_VerusUIID = XUnitTestUtil::NewUIID();
		m_PlayerUIID = XUnitTestUtil::NewUIID();

		// Net Player
		TD_UPDATE_CACHE_PLAYER td_update_cache_player;
		td_update_cache_player.uid = m_PlayerID;
		td_update_cache_player.nUIID = m_PlayerUIID;
		td_update_cache_player.vPos = m_vStartPosPlayer;
		td_update_cache_player.svDir = m_vStartDirPlayer;
		td_update_cache_player.nStance = CS_NORMAL;
		td_update_cache_player.nStatusFlag = 0;
		td_update_cache_player.nMF = MF_SWALLOWED;
		td_update_cache_player.nMFWeight = 500;
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

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_VerusMUID;
		td_update_cache_npc.nUIID = m_VerusUIID;
		td_update_cache_npc.nNPCID = 112001;
		td_update_cache_npc.vPos = m_vStartPos;
		td_update_cache_npc.svDir = m_vStartDir;
		td_update_cache_npc.nTalentID = 211200106;
		td_update_cache_npc.uidTarget = m_PlayerID;

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

	void TestSwallowedSynchStart()
	{
		_DamageRepeatInfo damageInfo;

		damageInfo.nTalentID	= 211200106;
		damageInfo.nMFWeight			= 500;	
		damageInfo.nAttackerUIID		= m_VerusUIID;

		XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_PlayerID);
		if(pNet)
		{
			XModuleNetControl* pNetActorControl = pNet->GetModuleNetControl();
			pNetActorControl->WaitSwallowedSynch(100);
		}

		XNonPlayer* pNPC = gg.omgr->FindNPC(m_VerusMUID);
		if(pNPC)
		{			
			XModuleActorControl* pNPCActorControl = pNPC->GetModuleActorControl();
			pNPCActorControl->OnSwallowedSynch(211200106, m_VerusMUID);
		}
	}

	void TestSpewstart()
	{
		minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
		pNetAgent->OnRecv( MC_NPC_SPEWUP, 1,
			NEW_UID( m_VerusMUID ));
	}

	virtual void OnCmd(const std::vector< std::wstring >& _refConsoleCmd)
	{
		_ASSERT(_refConsoleCmd.empty() == false);
		if (_refConsoleCmd[0] == L"init")
		{
			InitSwallowedSynch();
		}
		else
		{
			if (false == m_bContextUsable)
			{
				m_pEngineDebugger->PrintToConsole(L"먹기 테스트 할수 있는 환경 아님 - 'ed sw init' 요망");
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 서브 커맨드
			//
			if (_refConsoleCmd[0] == L"a")
			{
				TestSwallowedSynchStart();
				return;
			}
			if(_refConsoleCmd[0] == L"e")
			{
				TestSpewstart();
				return;
			}
		}
	}

	bool m_bNeedToReset;

	MUID m_VerusMUID;
	MUID m_PlayerID;

	UIID m_VerusUIID;
	UIID m_PlayerUIID;

	int m_nTalentID;
	int m_nGripPhase;
	float m_fElapsed;

	RVector m_vStartPos;
	RVector m_vStartDir;

	RVector m_vStartPosPlayer;
	RVector m_vStartDirPlayer;
};
