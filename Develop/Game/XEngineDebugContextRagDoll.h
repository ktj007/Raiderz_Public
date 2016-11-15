#pragma once

#include "XEngineDebugContext.h"
#include "XBaseApplication.h"
#include "XGameTransformControllerManager.h"
#include "XModuleMovable.h"
#include "XModuleActorControl.h"
#include "XGlobal.h"
#include "XUISystem.h"

#include "XPhysX.h"
#include "MPhysX4R3/RPhysXRagDollBuilder.h"
#include "MPhysX4R3/MPxRagDollTC.h"

#include "XUnitTestUtil.h"

class XEngineDebugContextRagDoll : public XEngineDebugContext
{
public:
	XEngineDebugContextRagDoll()
		: XEngineDebugContext(XEngineDebugContext::DCT_FREE_WORLD)
	{
		m_bNeedToReset = false;

		m_nGripPhase = 0;

		m_vStartPos.Set(500,500,0);
		m_vStartDir.Set(1,1,0);
		m_vStartDir.Normalize();

		m_vStartPosRag = m_vStartPos + (300.f * m_vStartDir);
		m_vStartDirRag = -m_vStartDir;

		m_nTalentID = 211015;
	}

	virtual ~XEngineDebugContextRagDoll(){}

	virtual void OnUpdate(float fDelta)
	{
		XObject* pOgre = gg.omgr->Find(m_NPCID);
		XObject* pRagdoll = gg.omgr->Find(m_RAGID);

		if (m_nGripPhase == 1)
		{
			pOgre->SetPosition(m_vStartPos);
			pOgre->SetDirection(m_vStartDir);

			pRagdoll->SetPosition(m_vStartPosRag);
			pRagdoll->SetDirection(m_vStartDirRag);

			m_nGripPhase = 2;
		}
		else if (m_nGripPhase == 2)
		{
			minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();
			pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,
				NEW_USHORT( m_NPCUIID ),
				NEW_INT( m_nTalentID ),
				NEW_SVEC(pOgre->GetDirection() ),
				NEW_VEC(pOgre->GetPosition() )
				);

			pNetAgent->OnRecv( MC_ACTION_EXTRA_ACT_TALENT, 4,
				NEW_USHORT( m_NPCUIID ),
				NEW_INT( m_nTalentID ),
				NEW_VEC(pOgre->GetPosition() ),
				NEW_SVEC(pOgre->GetDirection() )
				);

			m_nGripPhase = 3;
			m_fElapsed = 1.f;
		}
		else if (m_nGripPhase == 3)
		{
			if (m_fElapsed > 0)
			{
				m_fElapsed += (fDelta * 1000);
				if (m_fElapsed > 50.f)
				{
					minet::MTestNetAgent* pNetAgent = m_pEngineDebugger->GetNetAgent();

					TD_TALENT_HIT	td_talent_hit;
					td_talent_hit.nAttackerUIID = m_NPCUIID;
					td_talent_hit.nVictimUIID = m_RAGUIID;
					td_talent_hit.nTalentID = m_nTalentID;
					td_talent_hit.nDamage = 100;
					td_talent_hit.nFlags = 0;
					td_talent_hit.nMFState = MF_GRAPPLED;
					td_talent_hit.nMFWeight = 500;
					td_talent_hit.nHitTime = 0;

					pNetAgent->OnRecv(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));
					m_fElapsed = -1.f;
				}
			}

			XModuleTalent* pTalent = pOgre->GetModuleTalent();
			if (pTalent->IsActive() == false)
			{
				m_nGripPhase = 0;
			}
		}

		UpdateMouseControl(pRagdoll);
	}

	void UpdateMouseControl( XObject* pRagdoll )
	{
		if (global.app->IsActive())
		{
			int nDeltaX=0, nDeltaY=0;
			POINT pt;

			GetCursorPos(&pt);
			ScreenToClient(global.app->GetWindowHandle(),&pt);

			static POINT last_pt = pt;

			nDeltaX = pt.x - last_pt.x;
			nDeltaY = pt.y - last_pt.y;

			last_pt = pt;

			if (nDeltaX < -100) nDeltaX = -100;
			if (nDeltaY < -100) nDeltaX = -100;
			if (nDeltaX > 100) nDeltaX = 100;
			if (nDeltaX > 100) nDeltaX = 100;

			if (global.ui && global.ui->IsMouseVisible() && global.camera)
			{
				if (MEvent::GetLButtonState())
				{
					vec3 vPos = pRagdoll->GetPosition();

					vec3 vCamDir = global.camera->GetPrimaryCamera()->GetWorldDirection();
					vec3 right = vec3(0,0,1).CrossProduct(vCamDir);
					right.Normalize();

					right = right * (nDeltaX * -1.0f);

					vec3 vDelta = vec3::ZERO;
					vDelta.z = (nDeltaY * -1.0f);
					vDelta.x = right.x;
					vDelta.y = right.y;

					vPos = vPos + vDelta;

					pRagdoll->SetPosition(vPos);
				}


				if (MEvent::GetRButtonState())
				{
					XModuleEntity* pEntity = pRagdoll->GetModuleEntity();
					pEntity->GetAnimationController()->SetAnimation(wstring(L"ragdoll_default"));

					pRagdoll->GetModuleMovable()->SetActive(false);
				}

				if (MEvent::GetMButtonState())
				{
					static bool active = false;
					pRagdoll->GetModuleMovable()->SetActive(active);
					active = !active;
				}
			}
		}
	}
	void StartGrip()
	{
		m_nGripPhase = 1;
	}

	void InitRagDollWorld()
	{
		//global.phx->AddCollisionGroup("RagDoll", true);
		//global.phx->AddCollisionGroup("Cloth", false);
		//global.phx->AddCollisionGroup("Soft", false);

		// 월드
		XWorld* pWorld = gg.currentgamestate->GetWorld();

		XWorldInitParam worldInitParam;
		worldInitParam.SetZone(L"BirdTown", true, false);

		pWorld->Init(worldInitParam);

		m_NPCID = global.system->GenerateLocalUID();
		m_RAGID = global.system->GenerateLocalUID();

		m_NPCUIID = XUnitTestUtil::NewUIID();
		m_RAGUIID = XUnitTestUtil::NewUIID();

		// NPC
		TD_UPDATE_CACHE_NPC td_update_cache_npc;
		td_update_cache_npc.uid = m_NPCID;
		td_update_cache_npc.nUIID = m_NPCUIID;
		td_update_cache_npc.nNPCID = 110;
		td_update_cache_npc.vPos = m_vStartPos;
		td_update_cache_npc.svDir = m_vStartDir;

		gg.currentgamestate->InNPCs(&td_update_cache_npc, 1);

		// Net Player
		TD_UPDATE_CACHE_PLAYER td_update_cache_player;
		td_update_cache_player.uid = m_RAGID;
		td_update_cache_player.nUIID = m_RAGUIID;
		td_update_cache_player.vPos = m_vStartPosRag;
		td_update_cache_player.svDir = m_vStartDirRag;
		td_update_cache_player.nStance = CS_NORMAL;
		td_update_cache_player.nStatusFlag = 0;
		wcsncpy_s(td_update_cache_player.szName, L"ragdoll", _TRUNCATE);

		// feature
		td_update_cache_player.Feature.nSex = SEX_FEMALE;
		td_update_cache_player.Feature.nHair = 0;
		td_update_cache_player.Feature.nFace = 0;
		td_update_cache_player.Feature.nHairColor = 0;
		td_update_cache_player.Feature.nSkinColor = 0;
		td_update_cache_player.Feature.nMakeUp = 0;
		td_update_cache_player.Feature.nWeaponSet = 0;

		TD_PLAYER_FEATURE_TATTOO td_player_feature_tattoo;
		td_player_feature_tattoo.nTattooType = 0;
		td_player_feature_tattoo.nTattooPosX = 0;
		td_player_feature_tattoo.nTattooPosY = 0;
		td_player_feature_tattoo.nTattooScale = 0;
		
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
			InitRagDollWorld();
		}
		else
		{
			if (false == m_bContextUsable)
			{
				m_pEngineDebugger->PrintToConsole(L"랙돌을 테스트 할수 있는 환경 아님 - 'ed r init' 요망");
				return;
			}

			if (m_nGripPhase != 0)
			{
				m_pEngineDebugger->PrintToConsole(L"랙돌 테스트 작동중 - 왜이리 급하시나...");
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// 서브 커맨드
			//
			if (_refConsoleCmd[0] == L"a")
			{
				StartGrip();
				return;
			}
			else if (_refConsoleCmd[0] == L"reload")
			{
				//global.phx->ReloadRagDollInformation();

				//XActor* pActor = gg.omgr->FindActor(m_RAGID);
				//XModuleEntity* pEntity = pActor->GetModuleEntity();
				//MPxRagDollTC* pRagDollTransformController = pEntity->GetTransformController<MPxRagDollTC>();
				//pRagDollTransformController->ChangeRagDollFromContext("default");
			}
			else if (_refConsoleCmd[0] == L"density" && _refConsoleCmd.size() == 2)
			{
				float fDesity = _wtof(_refConsoleCmd[1].c_str());
				XActor* pActor = gg.omgr->FindActor(m_RAGID);
				XModuleEntity* pEntity = pActor->GetModuleEntity();
				MPxRagDollTC* pRagDollTransformController = pEntity->GetTransformController<MPxRagDollTC>();
				pRagDollTransformController->GetRagDollFactor()->fDensity = fDesity;
			}
			else if(_refConsoleCmd[0] == L"h")
			{
				XActor* pActor = gg.omgr->FindActor(m_RAGID);
				XModuleActorControl* pActorControl = pActor->GetModuleActorControl();

				pActorControl->OnBuffInstantEffectGain(750);
			}
		}
	}

	bool m_bNeedToReset;

	MUID m_NPCID;
	MUID m_RAGID;

	UIID m_NPCUIID;
	UIID m_RAGUIID;

	int m_nTalentID;
	int m_nGripPhase;
	float m_fElapsed;

	RVector m_vStartPos;
	RVector m_vStartDir;

	RVector m_vStartPosRag;
	RVector m_vStartDirRag;
};
