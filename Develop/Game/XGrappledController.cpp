#include "stdafx.h"
#include "XGrappledController.h"
#include "XModuleAction.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XModuleTalent.h"
#include "XEffectInvoker.h"
#include "XTalentHitParam.h"
#include "MPhysX4R3/MPxRagDollTC.h"
#include "XGameState.h"
#include "XTalentInfoMgr.h"
#include "XGameTransformControllerManager.h"

#ifdef _DEBUG
//#include "RDebugPassRenderer.h"
//#define GRAPLLED_DEBUG
#endif

XGrappledController::XGrappledController()
{
	m_pPlayer = NULL;
	m_pHitInfo = NULL;
	m_bAttach = false;
	m_bBeThrown = false;
	m_bCollision = false;
	m_bFinished = true;
	m_bStart = false;

	m_fRemainStartTime = 0.0f;

	m_strGrappledPlayerAnimation = MOTION_NAME_MF_GRAPPLED;
}


XGrappledController::~XGrappledController()
{
	m_pPlayer = NULL;
	SAFE_DELETE( m_pHitInfo);
}


void XGrappledController::SetOwnerPlayer( XPlayer* pPlayer)
{
	if ( pPlayer == NULL)
	{
		assert( 0);
		return;
	}


	m_pPlayer = pPlayer;
}


bool XGrappledController::OnStart( _DamageRepeatInfo* pHitInfo)
{
	// 변수 설정
	m_bStart = true;
	m_bBeThrown = false;
	m_bCollision = false;
	m_bFinished = false;

	m_fRemainStartTime = 0.0f;

	if ( m_pPlayer == NULL)
	{
		assert( 0);
		return false;
	}

	// 공격자 설정
	if ( m_pHitInfo)
		SAFE_DELETE( m_pHitInfo);

	m_pHitInfo = pHitInfo;
	if ( m_pHitInfo == NULL)
	{
		assert( 0);
		return false;
	}

	XObject* pAttacker = gg.omgr->FindActor_UIID( m_pHitInfo->nAttackerUIID);
	if ( pAttacker == NULL)
	{
		assert( 0);
		return false;
	}

	pAttacker->SetExtraTarget( m_pPlayer->GetUID());

	// 판정 정보 가져오기(위치, 시간)
	vec3 vHitPos = m_pPlayer->GetPosition();
	m_fRemainStartTime = GetGrappledHitTime(pAttacker);

	// 피공격자 설정
	XModuleMovable* pMovable = m_pPlayer->GetModuleMovable();
	if ( pMovable)
	{
		MUID uidAttacker = pAttacker->GetUID();

		pMovable->ForceStop();
		pMovable->TriggerGrappledStart( uidAttacker, (float)m_pHitInfo->nMFWeight, vHitPos, m_fRemainStartTime);
	}

	// 피공격자 애니메이션 선택
	XTalentInfo* pTalentInfo = info.talent->Get(pHitInfo->nTalentID);
	if(pTalentInfo)
	{
		if(pTalentInfo->m_strGrappledAni.empty() == false)
			m_strGrappledPlayerAnimation = pTalentInfo->m_strGrappledAni;
		if(pTalentInfo->m_strGrappledBone.empty() == false)
			m_strGrappledPlayerBoneName = pTalentInfo->m_strGrappledBone;
	}

	return true;
}


void XGrappledController::OnFinish()
{
	SAFE_DELETE( m_pHitInfo);

	m_bFinished = true;


	if ( m_pPlayer == NULL)
		return;


	XModuleEntity* pEntity = m_pPlayer->GetModuleEntity();
	if ( pEntity)
		pEntity->SetRenderPosDir( vec3::ZERO, vec3::ZERO);


	XModuleMovable* pMovable = m_pPlayer->GetModuleMovable();
	if ( pMovable)
		pMovable->TriggerGrappledEnd();
}

bool XGrappledController::IsValidAttacker(XPlayer* pAttacker)
{
	if(pAttacker && pAttacker->GetActor()->IsLoadingCompleted() == false)
	{
		//mlog("invalid attacker\n");
		return false;
	}
	return true;
}

void XGrappledController::OnUpdate( float fDelta)
{
	if ( m_pPlayer == NULL  ||  m_bFinished == true)
		return;

	m_fRemainStartTime -= fDelta;
	if(m_fRemainStartTime > 0.0f)
	{
		m_bStart = true;
		return;
	}
	
	if(m_bStart)
	{
		SetGrappledAnimation();
		m_bStart = false;
	}

	// 데미지 표시 업데이트
	_UpdateDamage(fDelta);


	// 업데이트
	XModuleMovable* pMovable = m_pPlayer->GetModuleMovable();
	XModuleEntity* pEntity = m_pPlayer->GetModuleEntity();
	XModuleMotion* pMotion = m_pPlayer->GetModuleMotion();

	const XMovableFactor::SState& GENERAL_STATE = pMovable->GetMovableFactor().GENERAL_STATE;
	const XMovableFactor::SGrapple& GRAPPLE = pMovable->GetMovableFactor().GRAPPLE;

	XObject* pObject = gg.omgr->Find( GRAPPLE.m_uidGrappledAttacker);
	XPlayer* pAttacker = NULL;
	if ( pObject != NULL)
		pAttacker = static_cast<XPlayer*>( pObject);
	
	if ( GRAPPLE.m_bGrappled == false || IsValidAttacker(pAttacker) == false )
	{
		OnFinish();
		return;
	}

	m_bAttach = GRAPPLE.m_bGrappledAttach;

	// 던져지기 전에 공격자의 상태 검사
	if ( m_bBeThrown == false)
	{
		// 아직 안던졌는데 공격자가 없어졌거나 죽었는지 검사
		if ( pObject == NULL  ||  pAttacker->IsDead())
		{
			OnFinish();
			return;
		}

		// 공격자가 현재 던지기 탈렌트(extra talent)를 사용하지 않고 있을 때 강제로 던지기를 시작한다
		XModuleTalent* pModuleTalent = pObject->GetModuleTalent();
		if ( pModuleTalent->IsActive() == false  ||  pModuleTalent->GetTalent()->GetInfo()->m_nSkillType != ST_EXTRA_ACTIVE)
		{
			OnBeThrown();
			pMovable->TriggerGrappledBeThrown();
		}
	}



	// 공격자가 던져졌는지 여부 - 랙이 심할경우 이벤트가 먼저 호출되는 경우가 발생하므로 여기서 처리함
	// 공격자가 죽어도 이벤트는 발생하므로 주의.
	if ( m_bBeThrown == true  &&  GRAPPLE.m_bGrappledBeThrown == false)
	{
		pMovable->TriggerGrappledBeThrown();
	}


	// 수영중이면 충돌했으면 종료함
	if ( GENERAL_STATE.m_bSwim == true)
	{
		OnFinish();
		return;
	}


	// 최초 충돌
	if ( GRAPPLE.m_bGrappledBeThrown == true  &&  m_bCollision == false  &&  GENERAL_STATE.m_bColTerrain == true)
	{
		m_bCollision = true;

		// 이펙트
		_ShowEffect( GRAPPLED_COLLISION_BASE_EFFECT_NAME, GRAPPLED_COLLISION_BASE_EFFECT_BONENAME);


		// 남은 데미지 표시
		int nDamage = m_pHitInfo->nDamage;
		for ( vector<_DamageRepeatEvent>::iterator itr = m_pHitInfo->vecDamageRepeatEvents.begin();  itr != m_pHitInfo->vecDamageRepeatEvents.end();  itr++)
			nDamage -= (*itr).nDamage;

		_ShowDamage( nDamage);


		// 착지 준비 모션
		pEntity->SetRenderPosDir( vec3::ZERO, vec3::ZERO);
		pMotion->ChangeMotion( GRAPPLED_COLLISION_PLAYER_ANIMATION, MT_DEFAULT, true);
		pMotion->PushMotion( GRAPPLED_DOWN_PLAYER_ANIMATION);
	}

	
	// 종료 체크
	if ( GRAPPLE.m_vGrappledVelocity == vec3::ZERO)
	{
		// 아직 충돌 모션 중이면 패쓰
		if ( pMotion->GetCurrMotion() == GRAPPLED_COLLISION_PLAYER_ANIMATION)
		{
			// 죽었으면 상태를 바로 바꿔준다
			if ( m_pPlayer->IsDead() == true)
			{
				pMotion->HoldSequence( true);
				OnFinish();

				// 방향 검사
				vec3 vCurrDir = m_pPlayer->GetDirection();
				if(vCurrDir.x == 0.0f &&
					vCurrDir.y == 0.0f)
					m_pPlayer->SetDirection(GRAPPLE.m_vGrappledEndDir);

				// 지형 노말로 죽어있자.
				Die();

				// by pok. '최초 충돌 - 착지 준비 모션'에서 가져온 코드. 죽었어도 착지모션을 취한다.
				// Ticket #1929 관련버그
				pEntity->SetRenderPosDir( vec3::ZERO, vec3::ZERO);
				pMotion->ChangeMotion( GRAPPLED_COLLISION_PLAYER_ANIMATION , MT_DEFAULT, true);
				pMotion->PushMotion( GRAPPLED_DOWN_PLAYER_ANIMATION);
			}
		}

		// 모션이 엎어져 있으면 일으켜 세우고
		else if ( pMotion->GetCurrMotion() == GRAPPLED_DOWN_PLAYER_ANIMATION)
		{
			pMotion->ChangeMotion( GRAPPLED_UP_PLAYER_ANIMATION, MT_DEFAULT, true);
			pMotion->PushMotion( MOTION_NAME_IDLE);

			return;
		}

		// 일어섰으면 종료한다
		else if ( pMotion->GetCurrMotion() == MOTION_NAME_IDLE)
		{
			OnFinish();
			return;
		}
	}
}


void XGrappledController::OnBeThrown()
{
	m_bBeThrown = true;
	m_bCollision = false;
}


bool XGrappledController::IsFinish()
{
	if ( m_pPlayer == NULL)
		return true;

	return m_bFinished;
}


vec3 XGrappledController::_GetSpineDummyPos(wstring strBoneName)
{
	vec3 v( 0.0f, 0.0f, 0.0f);
	if ( m_pPlayer == NULL)
		return v;

	XCharacter* pCharacter = m_pPlayer->GetActor();
	if ( pCharacter == NULL)
		return v;

	if (pCharacter->GetActorNodePos(strBoneName.c_str(), v, E_TS_LOCAL_RESULT))
		v.z = -v.z;

	return v;
}

void XGrappledController::_UpdateDamage( float fDelta)
{
	if ( m_pPlayer == NULL  ||  m_pHitInfo == NULL)
		return;


	for ( vector<_DamageRepeatEvent>::iterator itr = m_pHitInfo->vecDamageRepeatEvents.begin();  itr != m_pHitInfo->vecDamageRepeatEvents.end();  itr++)
	{
		if ( (*itr).fTime > 1000.0f)
			continue;


		(*itr).fTime -= fDelta;


		if ( (*itr).fTime <= 0.0f)
		{
			_ShowDamage( (*itr).nDamage);
			_ShowEffect( (*itr).strEffectName, (*itr).strBone);

			(*itr).fTime = 9999.0f;
		}
	}
}


void XGrappledController::_ShowDamage( int nDamage)
{
	if ( m_pPlayer == NULL  ||  m_pHitInfo == NULL)
		return;

	if ( nDamage <= 0)
		return;


	XModuleActorControl * pModuleActorControl = m_pPlayer->GetModuleActorControl();
	if ( pModuleActorControl)
	{
		_DamageInfo damageInfo;
		damageInfo.nAttackedTalentID	= m_pHitInfo->nTalentID;
		damageInfo.nDamage				= nDamage;
		damageInfo.nFlags				= m_pHitInfo->nFlags;
		damageInfo.nMFState				= m_pHitInfo->nMFState;
		damageInfo.nAttackerUIID		= m_pHitInfo->nAttackerUIID;

		pModuleActorControl->ShowHitAndDamage( &damageInfo, NULL, vec3( 0, 0, -120.0f));
	}
}


void XGrappledController::_ShowEffect( wstring strEffectName, wstring strBoneName)
{
	if ( m_pPlayer == NULL  ||  m_pHitInfo == NULL)
		return;

	if ( strEffectName.empty())
		return;


	// 이펙트가 안보일 것이다. 그건... 렉돌때문이다.
	if(m_pPlayer->GetModuleEntity())
	{
		if(m_pPlayer->GetModuleEntity()->GetTransformController<physx::MPxRagDollTC>()->IsEnabled())
		{
			XEffectSinglyInvoker effectInvoker;
			effectInvoker.Invoke(strEffectName, m_pPlayer->GetPosition(), (vec3)vec3::AXISY, (vec3)vec3::AXISZ);
		}
		else
		{
			XEffectInvoker effectInvoker;
			effectInvoker.Invoke( m_pPlayer, strEffectName, wstring(), strBoneName);
		}
	}
}

void XGrappledController::Die()
{
	XModuleAction* pModuleAction = m_pPlayer->GetModuleAction();
	if(pModuleAction)
	{
		pModuleAction->DoneDIe();
	}
}

void XGrappledController::OnPostUpdate( float fDelta )
{
	if(m_bStart)
		return;

	XModuleEntity* pEntity = m_pPlayer->GetModuleEntity();
	XModuleMovable * pMovable = m_pPlayer->GetModuleMovable();

	// Rag doll의 spine 위치를 맞춘다
	if ( (m_bBeThrown == false && m_bAttach == true ) ||  (m_bBeThrown == true  &&  m_bCollision == false))
	{
		wstring strBoneName = GRAPPLED_PLAYER_BONENAME;
		if(m_strGrappledPlayerBoneName.empty() == false)
			pEntity->SetRenderPosDir( _GetSpineDummyPos(m_strGrappledPlayerBoneName), vec3::ZERO );
		else
			pEntity->SetRenderPosDir( _GetSpineDummyPos(strBoneName), vec3::ZERO );
	}
}

float XGrappledController::GetGrappledHitTime( XObject* pAttacker )
{
	if ( pAttacker == NULL ||
		pAttacker->GetActor() == NULL)
		return 0.0f;

	XExtraActiveTalent_Grapple* pGrappleTalent = static_cast<XExtraActiveTalent_Grapple*>(pAttacker->GetModuleTalent()->GetTalent());
	if(pGrappleTalent == NULL)
		return 0.0f;

	for ( vector<_DamageRepeatEvent>::iterator itr = m_pHitInfo->vecDamageRepeatEvents.begin();  itr != m_pHitInfo->vecDamageRepeatEvents.end();  itr++)
		itr->fTime -= pGrappleTalent->m_fGrappledRemainTime;

	// Throw 모션 블렌딩중인가?
	if(pAttacker->GetModuleEntity()->GetAnimationController()->GetPreAnimationName().empty() == false)
	{
		int nPreAniFrame = pAttacker->GetModuleEntity()->GetAnimationController()->GetPreAniFrame();

		// 아직 Throw 모션이 아니인데... 히트 시간이 지났다면... 붙어라...
		if(nPreAniFrame >= pGrappleTalent->m_nGrappledHitFrame)
		{

			#ifdef GRAPLLED_DEBUG
				vec3 vCurPoint;
				pAttacker->GetActor()->GetActorNodePos(GRAPPLED_NPC_GRIP_BONE_NAME, vCurPoint, E_TS_WORLD, true);
				RMatrix mat;
				mat.MakeIdentity();
				mat.SetTranslation( vCurPoint);
				REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 5, 20, D3DCOLOR_XRGB(255, 0, 0));
			#endif

			return 0.0f;
		}


#ifdef GRAPLLED_DEBUG
		vec3 vCurPoint;
		pAttacker->GetActor()->GetActorNodePos(GRAPPLED_NPC_GRIP_BONE_NAME, vCurPoint, E_TS_WORLD, true);
		RMatrix mat;
		mat.MakeIdentity();
		mat.SetTranslation( vCurPoint);
		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 5, 20, D3DCOLOR_XRGB(0, 255, 0));
#endif

		int nRemainFrame = pGrappleTalent->m_nGrappledHitFrame - nPreAniFrame;
		return pAttacker->GetModuleEntity()->GetAnimationController()->GetPreAniFrameToSec(nRemainFrame);
	}

	return 0.0f;
}

void XGrappledController::SetGrappledAnimation()
{
	// 피공격자 모션 설정
	XModuleMotion* pMotion = m_pPlayer->GetModuleMotion();
	if ( pMotion)
	{
		if(info.motion->GetMotion(m_strGrappledPlayerAnimation.c_str()) == NULL)
		{
			// 애니메이션이다.
			pMotion->ChangeToCustomMotion(MOTION_NAME_CUSTOM, m_strGrappledPlayerAnimation.c_str(), true);
		}
		else
		{
			pMotion->ChangeMotion( m_strGrappledPlayerAnimation.c_str(), MT_DEFAULT, true);
		}
	}
}