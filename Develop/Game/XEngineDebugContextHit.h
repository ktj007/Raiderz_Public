#pragma once

#include "XEngineDebugContext.h"

class XEngineDebugContextHit : public XEngineDebugContext
{
public:
	XEngineDebugContextHit()
		: XEngineDebugContext(XEngineDebugContext::DCT_MY_CHARACTER_WORLD)
	{
		m_vStartPosNet.Set(500,500,0);
		m_vStartDirNet.Set(1,1,0);
		m_vStartDirNet.Normalize();

		m_vStartPosMy = m_vStartPosNet + (300.f * m_vStartDirNet);
		m_vStartDirMy = -m_vStartDirNet;
	}

	virtual ~XEngineDebugContextHit(){}

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

				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				XTalentHitParam talentHitParam;

				talentHitParam.nAttackerUIID				= m_NPCUIID;
				talentHitParam.vServerKnockbackDir			= m_vStartDirNet;
				talentHitParam.vServerKnockbackVictimPos	= m_vStartPosMy;
				talentHitParam.vServerKnockbackVictimTargetPos	= m_vStartPosMy + (m_vStartDirNet * MF_THROWUP);
				talentHitParam.nTalentID					= 210600154;
				talentHitParam.nMFState						= MF_THROWUP;
				talentHitParam.nMFWeight					= 500;
				talentHitParam.nDamage						= 100;
				talentHitParam.nFlags						= 0;

				pActorControl->OnHit(&talentHitParam);

				return;
			}
			else if (_refConsoleCmd[0] == L"b")
			{
				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				TD_BUFF_HIT buffInfo;
				buffInfo.nVictimUIID	= m_MyPlayerUIID;
				buffInfo.nBuffID	= 40050;
				buffInfo.nDamage	= 100;
				buffInfo.nMFWeight	= 500;
				buffInfo.nMFState	= MF_NONE;
				buffInfo.nFlags	    = 0;		

				pActorControl->OnBuffHit(& buffInfo, true);

				return;
			}
			else if (_refConsoleCmd[0] == L"c")
			{

				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				XTalentHitParam talentHitParam;

				talentHitParam.nAttackerUIID				= m_NPCUIID;
				talentHitParam.vServerKnockbackDir			= m_vStartDirNet;
				talentHitParam.vServerKnockbackVictimPos	= m_vStartPosMy;
				talentHitParam.vServerKnockbackVictimTargetPos	= m_vStartPosMy + (m_vStartDirNet * MF_NONE);
				talentHitParam.nTalentID					= 210600154;
				talentHitParam.nMFState						= MF_NONE;
				talentHitParam.nMFWeight					= 500;
				talentHitParam.nDamage						= 100;
				talentHitParam.nFlags						= 0;


				pActorControl->OnHit(&talentHitParam);

				return;
			}
			else if (_refConsoleCmd[0] == L"d")
			{

				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				TD_BUFF_HIT buffHit;
				buffHit.nBuffID = 40051;
				buffHit.nDamage = 50;
				buffHit.nFlags = 0;
				buffHit.nMFState = MF_NONE;
				buffHit.nMFWeight = 0;
				buffHit.nVictimUIID = pNet->GetUIID();

				pActorControl->OnBuffHit(&buffHit, true);

				return;
			}
			else if (_refConsoleCmd[0] == L"stun")
			{

				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				TD_BUFF_HIT buffHit;
				buffHit.nBuffID = 1553;
				buffHit.nDamage = 50;
				buffHit.nFlags = 0;
				buffHit.nMFState = MF_STUN;
				buffHit.nMFWeight = 10;
				buffHit.nVictimUIID = pNet->GetUIID();

				pActorControl->OnBuffHit(&buffHit, true);

				return;
			}
			else if (_refConsoleCmd[0] == L"t")
			{
				XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
				XModuleActorControl* pActorControl = pNet->GetModuleActorControl();

				pActorControl->OnUseTalent(10701, pNet->GetPosition(), pNet->GetDirection());

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

		m_MyPlayerID = global.system->GenerateLocalUID();
		m_NPCID = global.system->GenerateLocalUID();

		m_MyPlayerUIID = XUnitTestUtil::NewUIID();
		m_NPCUIID = XUnitTestUtil::NewUIID();

		// Net Player
		TD_UPDATE_CACHE_PLAYER td_update_cache_player;
		td_update_cache_player.uid = m_MyPlayerID;
		td_update_cache_player.nUIID = m_MyPlayerUIID;
		td_update_cache_player.vPos = m_vStartPosMy;
		td_update_cache_player.svDir = m_vStartDirMy;
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
		td_update_cache_player.Feature.nItemID[FEATURE_ITEMSLOT_RWEAPON] = 1;
		for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
			td_update_cache_player.Feature.nItemID_DyedColor[i] = 0;

		gg.currentgamestate->InPlayer(td_update_cache_player.uid, &td_update_cache_player);

		XNetPlayer* pNet = gg.omgr->FindNetPlayer(m_MyPlayerID);
		pNet->ChangeStance(CS_BATTLE, true);

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_NPCID;
		td_update_cache_npc.nUIID = m_NPCUIID;
		td_update_cache_npc.nNPCID = 106001;
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
