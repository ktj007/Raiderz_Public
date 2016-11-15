#pragma once

#include "XEngineDebugContext.h"
#include "XUnitTestUtil.h"

class XEngineDebugContextStun : public XEngineDebugContext
{
private:
	bool	m_bSorenaStun;
	float	m_fSorenaStunStart;

public:
	XEngineDebugContextStun()
		: XEngineDebugContext(XEngineDebugContext::DCT_MY_CHARACTER_WORLD)
	{
		m_vStartPosNet.Set(5000,5000,0);
		m_vStartDirNet.Set(1,1,0);
		m_vStartDirNet.Normalize();

		m_vStartPosMy = m_vStartPosNet + (300.f * m_vStartDirNet);
		m_vStartDirMy = -m_vStartDirNet;

		m_bSorenaStun = false;
		m_fSorenaStunStart = 0.0f;
	}

	virtual ~XEngineDebugContextStun(){}

	virtual void OnUpdate(float fDelta)
	{
		if(m_bSorenaStun)
		{
			m_fSorenaStunStart += fDelta;

			if(m_fSorenaStunStart > 1.0f)
			{
				// 스턴 버프
				minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
				XActor* pOgre = gg.omgr->FindActor(m_NetPlayerID);
				pNetAgent->OnRecv( MC_BUFF_GAIN, 3,
					NEW_USHORT( m_NetPlayerUIID ),
					NEW_INT( 331 ),
					NEW_FLOAT(10)
					);

				pNetAgent->OnRecv(MC_ACTION_BUFF_HIT_NO_MF, 2,
					NEW_USHORT(m_NetPlayerUIID),
					NEW_INT(331));

				m_fSorenaStunStart = 0.0f;
				m_bSorenaStun = false;
			}
		}
	}

	virtual void OnCmd(const std::vector< std::wstring >& _refConsoleCmd)
	{
		_ASSERT( _refConsoleCmd.empty() == false );

		if (_refConsoleCmd[0] == L"init")
		{
			InitStunDebuggingWorld();
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
			if (_refConsoleCmd[0] == L"a")
			{
				minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
				mlog("액션!");

				TD_TALENT_HIT	td_talent_hit;
				td_talent_hit.nAttackerUIID = m_MyPlayerUIID;
				td_talent_hit.nVictimUIID = m_NetPlayerUIID;
				td_talent_hit.nTalentID = 101001;
				td_talent_hit.nDamage = 10;
				td_talent_hit.nFlags = 0;
				td_talent_hit.nMFState = MF_STUN;
				td_talent_hit.nMFWeight = 5000;
				td_talent_hit.nHitTime = 0;

				pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

				pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
					NEW_USHORT( m_NetPlayerUIID ),
					NEW_INT( 101001 ),
					NEW_SVEC( m_vStartDirNet ),
					NEW_VEC( m_vStartPosNet )
					);

				return;
			}
			else if (_refConsoleCmd[0] == L"b")
			{
				minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
				mlog("액션!");

				TD_TALENT_HIT	td_talent_hit;
				td_talent_hit.nAttackerUIID = m_MyPlayerUIID;
				td_talent_hit.nVictimUIID = m_NetPlayerUIID;
				td_talent_hit.nTalentID = 101001;
				td_talent_hit.nDamage = 10;
				td_talent_hit.nFlags = 0;
				td_talent_hit.nMFState = MF_STUN;
				td_talent_hit.nMFWeight = 5000;
				td_talent_hit.nHitTime = 0;

				pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

				return;
			}
			else if(_refConsoleCmd[0] == L"c")
			{
				minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();

				pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
					NEW_USHORT( m_NetPlayerUIID ),
					NEW_INT( 210500220 ),
					NEW_SVEC( m_vStartDirNet ),
					NEW_VEC( m_vStartPosNet )
					);

				m_bSorenaStun = true;
				return;
			}
		}
	}

	void InitStunDebuggingWorld()
	{
		// ID 생성
		m_MyPlayerID = global.system->GenerateLocalUID();
		m_NetPlayerID = global.system->GenerateLocalUID();
		m_MyPlayerUIID = XUnitTestUtil::NewUIID();
		m_NetPlayerUIID = XUnitTestUtil::NewUIID();

		// 게임생성(월드 + 캐릭터)
		gvar.World.SetFieldID(9001);

		TD_MYINFO MyInfo;
		wcsncpy_s(MyInfo.szName, L"pok", _TRUNCATE);
		wcsncpy_s(MyInfo.szSurname, L"test_pok", _TRUNCATE);

		MyInfo.nSex	= SEX_FEMALE;
		MyInfo.nLevel	= 34;
		MyInfo.nMoney	= 999;
		MyInfo.nMaxHP	= 70;
		MyInfo.nMaxEN	= 90;
		MyInfo.nMaxSTA = 140;
		MyInfo.nXP		= 11053;
		MyInfo.nSTR	= 3;
		MyInfo.nDEX	= 1;
		MyInfo.nINT	= 1;
		MyInfo.nCHA	= 3;
		MyInfo.nCON	= 7;

		MyInfo.nCurHP	= 70;
		MyInfo.nCurEN	= 90;
		MyInfo.nCurSTA	= 140;

		MyInfo.fExpPercent	= 70;
		MyInfo.nRemainTP	= 99;

		vector<TD_ITEM> vecTDInventory;
		vector<TD_ITEM> vecTDEquipment;
		vector<int> vecTalentID;		
		vector<TD_PLAYERQUEST> vecTDPlayerQuest;
		TD_PALETTE tdPalette;
		vector<TD_FACTION> vecTDFaction;		

		gvar.MyInfo.SetFrom_TD_MYINFO(MyInfo, vecTDInventory, vecTDEquipment, vecTalentID, vecTDPlayerQuest, tdPalette, vecTDFaction, vector<int>());
		gvar.MyInfo.MyUID = m_MyPlayerID;
		gvar.MyInfo.SetUIID(m_MyPlayerUIID);

		gg.game->Create();

		// 내 캐릭터 이동
		minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
		pNetAgent->OnRecv( MC_ENTITY_WARP, 3, NEW_UID(m_MyPlayerID), NEW_VEC(m_vStartPosMy), NEW_VEC(m_vStartDirMy));

		//// Player
		//TD_UPDATE_CACHE_PLAYER td_update_cache_player;

		//td_update_cache_player.uid = m_NetPlayerID;
		//td_update_cache_player.nUIID = m_NetPlayerUIID;
		//td_update_cache_player.vPos = m_vStartPosNet;
		//td_update_cache_player.svDir = m_vStartDirNet;
		//td_update_cache_player.nStance = CS_NORMAL;
		//td_update_cache_player.nStatusFlag = 0;
		//strcpy_s(td_update_cache_player.szName, "test_net");

		//// feature
		//td_update_cache_player.Feature.nSex = SEX_FEMALE;
		//td_update_cache_player.Feature.nHair = 0;
		//td_update_cache_player.Feature.nFace = 0;
		//td_update_cache_player.Feature.nHairColor = 0;
		//td_update_cache_player.Feature.nSkinColor = 0;
		//td_update_cache_player.Feature.nWeaponSet = 0;
		//for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		//	td_update_cache_player.Feature.nItemID[i] = 0;
		//for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		//	td_update_cache_player.Feature.nItemID_DyedColor[i] = 0;

		//gg.currentgamestate->InPlayer(td_update_cache_player.uid, &td_update_cache_player);

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_NetPlayerID;
		td_update_cache_npc.nUIID = m_NetPlayerUIID;
		td_update_cache_npc.nNPCID = 105002;
		td_update_cache_npc.vPos = m_vStartPosNet;
		td_update_cache_npc.svDir = m_vStartDirNet;

		gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);

		// 카메라 설정
		global.camera->SetPrimaryCamera(CAMERA_MAIN_BACKVIEW);
		XCameraBackViewController* pCameraController = global.camera->GetCameraController<XCameraBackViewController>();
		if (pCameraController)
			pCameraController->Init(XGetMyUID());

		// 디버그 컨텍스트 활성화
		m_bContextUsable = true;

		// 활성화 직후 콘솔로 포커스
		global.ui->ToggleConsole();
		global.ui->ToggleConsole();
	}

	MUID m_NetPlayerID;
	MUID m_MyPlayerID;

	UIID m_NetPlayerUIID;
	UIID m_MyPlayerUIID;

	RVector m_vStartPosNet;
	RVector m_vStartDirNet;

	RVector m_vStartPosMy;
	RVector m_vStartDirMy;
};
