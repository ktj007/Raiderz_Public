#include "stdafx.h"
#include "XModuleActorControl.h"
#include "XModuleEntity.h"
#include "XModuleAction.h"
#include "XModuleTalent.h"
#include "XModuleMovable.h"
#include "XModuleCollision.h"
#include "XModuleBuff.h"
#include "XModuleEffect.h"
#include "XModuleCollision.h"
#include "XModuleBParts.h"
#include "XObject.h"
#include "XTalentInfo.h"
#include "XGlueGameEvent.h"
#include "XPlayer.h"
#include "XNonPlayer.h"
#include "XEventID.h"
#include "XTarget.h"
#include "XSystem.h"
#include "XControlCmd.h"
#include "XTalentInfoMgr.h"
#include "XTalentHelper.h"
#include "XModuleUI.h"
#include "XMMCode.h"
#include "XController.h"
#include "XTalentEvent.h"
#include "XCharacter.h"
#include "XTalentHitParam.h"
#include "XProjectileController.h"
#include "XStrings.h"
#include "XEffectInvoker.h"

XModuleActorControl::XModuleActorControl(XObject* pOwner) : XModule(pOwner)
{
	//_ASSERT(MIsDerivedFromClass(XPlayer, pOwner));
	m_bDoneMagicHitEffect	= true;
	m_bReservedHitByTalent	= false;
	m_eReservedSwimState	= SS_NONE;
	m_nReservedHitByTalentCount = 0;
	m_nApplicationDamageHpByTalentCount = 0;

	m_pDamageInfo = NULL;

	m_pOverlappedEquipmentController = NULL;

	if(pOwner->GetEntityType() == ETID_PLAYER)
		m_DuelFightChecker.Init(pOwner->AsPlayer());
}

XModuleActorControl::~XModuleActorControl(void)
{
	SAFE_DELETE(m_pOverlappedEquipmentController);
}

void XModuleActorControl::OnInitialized()
{
	XModule::OnInitialized();

	m_pModuleEntity = m_pOwner->GetModuleEntity();
	_ASSERT(m_pModuleEntity);

	m_pModuleAction = m_pOwner->GetModuleAction();
	_ASSERT(m_pModuleAction);

	m_pModuleTalent = m_pOwner->GetModuleTalent();
	_ASSERT(m_pModuleTalent);

	m_pModuleMovable = m_pOwner->GetModuleMovable();
	_ASSERT(m_pModuleMovable);

	m_pModuleBuff = m_pOwner->GetModuleBuff();
	_ASSERT(m_pModuleBuff);
}

//----------------------------------------------------------------------------//
// 서버와의 통신
void XModuleActorControl::OnActTalent( int nTalentID, vec3& vDir )
{
	m_pModuleTalent->Act();

	if ( m_pOwner->GetUID() == XGetMyUID())
		gvar.MyInfo.Talent.CoolTime.UseTalent( nTalentID);
}

// 자기의 스킬 충돌 영역 표시를 위해서 탈렌트를 세팅해주는 것이다.
// 문제가 있으면 다시 주석해 주삼. [6/19/2007 madduck]
void XModuleActorControl::OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir)
{
	m_pModuleTalent->SetTalentServerStartMatrix(MMatrix::IDENTITY);

	if ( nTalentID == 0)
		return;

	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo( nTalentID, m_pOwner);
	if ( pTalentInfo == NULL)
		return;


	m_pModuleMovable->TriggerTalentActive(pTalentInfo->m_nID);

	MMatrix matStart;
	matStart.SetLocalMatrix(vPos, -vDir, vec3::AXISZ);
	m_pModuleTalent->SetTalentServerStartMatrix(matStart);
	//mlog("use_talent: pos=%.1f %.1f %.1f, dir=%.1f %.1f %.1f\n",
	//	vPos.x, vPos.y, vPos.z, 
	//	vDir.x, vDir.y, vDir.z
	//	);

	// My player
	if ( m_pOwner->GetUID() == XGetMyUID())
	{
		if ( pTalentInfo->m_fCastingTime > 0.0f)
			gvar.MyInfo.Talent.CoolTime.CastingTalent( nTalentID);
		else
		{
			gvar.MyInfo.Talent.CoolTime.UseTalent( nTalentID);
			
			// 내 캐릭터는 캐스팅 시간이 없는 탈랜트를 다시 사용하지 않음.
			return;
		}
	}

	// Net player
	else if ( m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		// Net player가 normal 스탠스인데 전투상에만 가능한 스탠스이면 그냥 전투 스탠스로 바꿔줌
		XPlayer* pPlayer = static_cast<XPlayer*>( m_pOwner);
		if ( nTalentID == CSTalentInfoHelper::NormalSwordDrawingTalentID( pPlayer->GetCurrWeaponType()))
			pPlayer->SetStance( CS_BATTLE);

		if ( pPlayer->GetStance() == CS_NORMAL &&
			 CheckBitSet( pTalentInfo->m_nEnabledStanceFlag, CS_BATTLE) &&
			!CheckBitSet( pTalentInfo->m_nEnabledStanceFlag, CS_NORMAL) )
			pPlayer->SetStance( CS_BATTLE);
	}

	// Non player
	else
	{
	}


	m_pModuleTalent->Start( nTalentID);
}

void XModuleActorControl::OnCancelTalent()
{
	m_pModuleTalent->Cancel();
}

void XModuleActorControl::OnActSpellProjectile( int nTalentID, vec3& vEntityDir, vec3& vTargetPos, MUID& uidTarget, int nGroupID, int nCapsuleID )
{
	// 타겟 설정
	m_pModuleTalent->SetTarget(uidTarget, nGroupID, nCapsuleID);

	OnActTalent(nTalentID, vEntityDir);
}

void XModuleActorControl::OnActSpellMagicArea(int nTalentID, vec3& vTargetPos)
{
	OnActTalent(nTalentID, m_pOwner->GetDirection());
}

void XModuleActorControl::OnActArchery( int nTalentID, vec3& vDir, MUID& uidTarget )
{
	OnActTalent(nTalentID, vDir);
}

void XModuleActorControl::OnActArcheryDetail( int nTalentID, vec3& vDir, MUID& uidTarget, int nGroupID, int nCapsuleID )
{
	OnActTalent(nTalentID, vDir);
}

void XModuleActorControl::OnHit(XTalentHitParam* pHitInfo)
{
	MF_STATE nMFState = pHitInfo->nMFState;

	{
		float fTalentActStartElapsedTime = 0.0f;
		XObject* pAttacker = gg.omgr->FindActor_UIID(pHitInfo->nAttackerUIID);
		if (pAttacker && 
			pAttacker->GetModuleTalent() &&
			pAttacker->GetModuleTalent()->GetTalent())
		{
			fTalentActStartElapsedTime = pAttacker->GetModuleTalent()->GetTalent()->GetActElapsedTime();
		}

		// PVP 체크
		m_DuelFightChecker.SetDuel(pAttacker);
		
		uint32 delay = pHitInfo->MakeDelayTime(fTalentActStartElapsedTime);

		m_bReservedHitByTalent = true;
		m_nReservedHitByTalentCount ++;

		XTalentHitParam* pEventTalentHitParam = pHitInfo->Clone();
		global.emgr->AddHitInfo(pEventTalentHitParam, delay);

		XEvent msg( XEVTD_TALENT_HIT, pEventTalentHitParam);
		m_pOwner->PostDelayEvent( msg, delay);
		//mlog("히트 딜레이? %d", delay);
	}

	//------------------------------------------------------------------------
	// 모션팩터가 걸렸을때 처리
	if ( MF_BEATEN <= nMFState && MF_GRAPPLED >= nMFState )
	{
		ReleaseOnMF();
	}
}
//----------------------------------------------------------------------------//
// 클라와의 통신

void XModuleActorControl::ActorRotation(vec3& CameraDir)
{
	vec3 planedir = CameraDir;
	planedir.z = 0.0f;

	float gap = planedir.DotProduct(CameraDir);
	float acss = acosf(gap);

	if(gap>0.9999f)
		acss = 0.f;
	else if(gap<-0.9999f)
		acss = D3DX_PI;

	vec3 cross = planedir.CrossProduct(CameraDir);
	if(cross.y < 0.0f)
		acss = -acss;

	// 흠 이런...
	if( CameraDir.x < 0.0f)
		acss = -acss;

	SetActorSpineTarget(acss);
}

void XModuleActorControl::SetActorSpineTarget(float fAngle1/* =0.f */,float fAngle2/* =0.f */)
{
	if(m_pModuleEntity->GetActor())
	{
		m_pModuleEntity->GetActor()->SetActorSpineTarget(fAngle1, fAngle2);
	}
}

void XModuleActorControl::RemoveSpineController()
{
	if(m_pModuleEntity->GetActor())
	{
		m_pModuleEntity->GetActor()->SetActorSpineTarget(0, 0);
		m_pModuleEntity->GetActor()->RemoveActorSpineController();
	}
}

void XModuleActorControl::Stop()
{
	m_pModuleAction->Stop();
}

//액션 FSM 안에서 이걸 부르지 않도록 주의 
void XModuleActorControl::StopMove(bool bForced/* =false */)
{
	m_pModuleAction->StopMove(bForced);
}

bool XModuleActorControl::IsCurAiming()
{
	return m_pModuleAction->IsCurAiming();
}

vec3 XModuleActorControl::GetAimingDir()
{
	return m_pModuleAction->GetAimingDir();
}

bool XModuleActorControl::UseTalent(int nTalentID)
{
	m_pModuleTalent->SetTalentServerStartMatrix(m_pOwner->GetWorldTransform());
	return m_pModuleTalent->Start(nTalentID);
}


//void XModulePlayerControl::UpdateTargetList()
//{
//	XModuleTalent* pModuleTalent = m_pOwner->GetModuleTalent();
//	if(pModuleTalent && pModuleTalent->GetTalent() && pModuleTalent->GetTalent()->GetTarget())
//	{
//		pModuleTalent->GetTalent()->UpdateTargetList();
//	}
//}


void XModuleActorControl::OnBreakParts(MUID& uidAttacker, int nPartsIndex)
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	XModuleBParts* pParts = pActor->GetModuleBParts();
	if (pParts)
	{
		pParts->SetBreakableParts(uidAttacker, nPartsIndex);
	}
}

void XModuleActorControl::OnBreakPartsRecovery()
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	XModuleBParts* pParts = pActor->GetModuleBParts();
	if (pParts)
	{
		pParts->BreakPartsRecovery();
	}
}

void XModuleActorControl::OnBreakParts(int nPartsIndex, float fXYForce, float fZForce)
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	XModuleBParts* pParts = pActor->GetModuleBParts();
	if (pParts)
	{
		pParts->SetBreakableParts(nPartsIndex, fXYForce, fZForce);
	}
}

void XModuleActorControl::OnBreakPartEx( MUID& uidAttacker, int nPartsIndex, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount )
{
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	XModuleBParts* pParts = pActor->GetModuleBParts();
	if (pParts)
	{
		pParts->SetBreakablePartEx(uidAttacker, nPartsIndex, pUsableBpart, nUsableBpartCount);
	}
}

void XModuleActorControl::OnReCastingTalent( int nTalentID, int nStep )
{
	m_pModuleTalent->ReCastingTalent(nTalentID,nStep);
}


void XModuleActorControl::ReleaseOnMF()
{
	//------------------------------------------------------------------------
	// 피격시에 대한 기타 처리

	// 각 피격시에 대한 루팅처리
	OnHitLooting();
}

void XModuleActorControl::OnBuffTalentGain( int nBuffID, float fLostRemained)
{
	m_pModuleBuff->BuffGain(nBuffID, fLostRemained);
}

void XModuleActorControl::OnBuffTalentLost( int nBuffID )
{
	// 임시저장속에 있는지 체크하자
	if(m_pModuleBuff->CheckTempRemainBuffList(nBuffID))
		return;

	// 장비 오버랩 탤런트 사용중인지 체크
	if(m_pOverlappedEquipmentController &&
		m_pOverlappedEquipmentController->CheckBuffLost(m_pOwner, nBuffID) == false)
		return;

	m_pModuleBuff->BuffLost(nBuffID);
}

void XModuleActorControl::OnBuffInstantEffectGain( int nBuffID )
{
	m_pModuleBuff->GainInstantEffect(nBuffID);
}

void XModuleActorControl::OnBuffHit( int nBuffID )
{
	m_pModuleBuff->BuffHit(nBuffID);
}

void XModuleActorControl::OnBuffHit( TD_BUFF_HIT* pBuffHitInfo, bool bUseDamage )
{
	OnBuffHit(pBuffHitInfo->nBuffID);

	XBuffInfo* pInfo = info.buff->Get(pBuffHitInfo->nBuffID);
	if(pInfo == NULL ||
		bUseDamage == false)
		return;

	// 모션팩터
	if(pBuffHitInfo->nMFState == MF_DEFLECT ||
		pBuffHitInfo->nMFState == MF_STUN ||
		pBuffHitInfo->nMFState == MF_KNOCKDOWN ||
		pBuffHitInfo->nMFState == MF_BEATEN ||
		pInfo->m_bShowHitReaction == true)
	{
		OnHitByBuff(pBuffHitInfo);
	}

	// 데미지 정보
	_DamageInfo damageInfo;
	damageInfo.nDamageFlag	= DF_BUFF;
	damageInfo.nDamage		= pBuffHitInfo->nDamage;
	damageInfo.nFlags		= pBuffHitInfo->nFlags;
	damageInfo.nMFState		= (MF_STATE)pBuffHitInfo->nMFState;
	damageInfo.strName		= pInfo->GetName();

	ShowHitAndDamage(&damageInfo, NULL);
}

void XModuleActorControl::OnWarp(const vec3& vPos, const vec3& vDir)
{
	// 워프는 다음 조건에 의해서 워프를 한다.
	// 1.처음에 맵에 진입시 위치를 셋팅한다.
	// 2.캐릭터가 워프를 한다.
	// 3.이동시 예외처리에 의해서 워프를 한다.
	m_pOwner->SetPosition(vPos);
	m_pOwner->SetDirection(vDir);

	if(m_pModuleMovable)
		m_pModuleMovable->Warp(vPos, vDir);

	if (m_pModuleEntity)
		m_pModuleEntity->OnWarp();
}

void XModuleActorControl::StartAppear()
{
	if(m_pModuleEntity)
	{
		m_pModuleEntity->SetVisibility(0.0f);
		m_pModuleEntity->StartFade(1.0f, APPEAR_TIME);
	}
}

void XModuleActorControl::StartDisappear()
{
	if(m_pModuleEntity)
	{
		// AlphaTestFade가 작동했는지 확인해 보자
		float fNoiseRef = m_pModuleEntity->GetActor()->GetNoiseRef();

		// 작동중이라... visibility 까지 건드릴필요가 없다.
		if(fNoiseRef == 0.0f)
			return;

		m_pModuleEntity->StartFade(0.0f, DISAPPEAR_TIME);
	}
}

void XModuleActorControl::GuardDefense( TD_TALENT_GUARD* pTalentGuardInfo )
{
	XTalentGuardDefenseParam* pInfo	= new XTalentGuardDefenseParam();
	pInfo->Assign(pTalentGuardInfo);

	float fTalentActStartElapsedTime = 0.0f;
	XObject* pAttacker = gg.omgr->FindActor_UIID(pTalentGuardInfo->nAttackerUIID);
	if (pAttacker && 
		pAttacker->GetModuleTalent() &&
		pAttacker->GetModuleTalent()->GetTalent())
	{
		fTalentActStartElapsedTime = pAttacker->GetModuleTalent()->GetTalent()->GetActElapsedTime();
	}

	uint32 delay = pInfo->MakeDelayTime(fTalentActStartElapsedTime);

	XEvent msg( XEVTD_GUARD_DEFENSE, pInfo);
	m_pOwner->PostDelayEvent(msg, delay);
}

void XModuleActorControl::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();
	Subscribe(XEVTD_GUARD_DEFENSE);
	Subscribe(XEVTL_SWIM_IN);
	Subscribe(XEVTL_SWIM_OUT);

	Subscribe(XEVTD_MESH_LOADING_COMPLETE);
}

XEventResult XModuleActorControl::OnEvent( XEvent& msg )
{
	switch(msg.m_nID)
	{
	case XEVTD_GUARD_DEFENSE:
		{
			XTalentGuardDefenseParam* pInfo	= static_cast<XTalentGuardDefenseParam*>(msg.m_pData);
			OnGuardDefense(pInfo);

			SAFE_DELETE(pInfo);
		}
		return MR_TRUE;
	//case XEVTL_MOTION_PLAYONCE:
	//	{

	//	}
	//	return MR_FALSE;
	case XEVTL_SWIM_IN:
		{
			bool bFallingSwimStart = *(bool*)(msg.m_pData);
			CheckSwim(true, bFallingSwimStart);
		}
		return MR_TRUE;
	case XEVTL_SWIM_OUT:
		{
			CheckSwim(false);
		}
		return MR_TRUE;
	case XEVTD_MESH_LOADING_COMPLETE:
		{
			if(m_pModuleEntity->GetActor())
				m_pModuleEntity->GetActor()->SetAO(false);
		}
		return MR_FALSE;
	}
	return MR_FALSE;
}


void XModuleActorControl::OnGuardDefense( XTalentGuardDefenseParam* pTalentGuardInfo )
{
	// 이미 죽었는지 체크
	if(m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
		if(pPlayer->IsDead())
		{
			return;
		}
	}

	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(pTalentGuardInfo->nAttackerTalentID, m_pOwner);
	if (pModuleCollision && pTalentInfo)
	{
		XObject* pAttacker = gg.omgr->FindActor_UIID(pTalentGuardInfo->nAttackerUIID);
		vec3 vHit = pModuleCollision->GetMeleeHitEffectPosition(pAttacker, pTalentGuardInfo->nAttackerTalentID);

		if(pTalentInfo->m_nSkillType == ST_MAGIC &&
			pTalentInfo->m_Events.HasEvent(TE_LAUNCH_PROJECTILE))
		{
			// 마법 발사체이다.
			XProjectileController projectileController;
			projectileController.ProjectileDefence(m_pOwner, pAttacker, pTalentInfo);
		}
		else 
		{
			m_pModuleTalent->DefenseEffect(pTalentGuardInfo->nAttackerTalentID, pAttacker, vHit);
		}
	}

	// 부분 방어라며...
	if(pTalentGuardInfo->eGuardType == GT_PARTIAL)
	{
		OnGuardKnockBack(pTalentGuardInfo->vGuarderDir);

		// 부분 방어는 데미지가 있다.
		XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
		if(pModuleEffect)
		{
			XCaptionEffectEventData* pEventData = new XCaptionEffectEventData;
			pEventData->m_eGuardType = pTalentGuardInfo->eGuardType;
			pEventData->m_nParam1 = pTalentGuardInfo->nDamage;
			pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_GUARDDEFENSE, pEventData);
		}
	}
	else
	{
		OnGuarded();
	}

	// 방어가 100% 성공이 되기 때문에 이제 성공 메시지는 안보여주기로 함.
	// - http://dev:8181/projects/MyPrj/ticket/1496
	//XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	//if(pModuleEffect)
	//{
	//	pModuleEffect->ShowGuardDefense(pTalentGuardInfo->eGuardType, pTalentGuardInfo->nDamage);
	//}
}

void XModuleActorControl::OnExtraActTalent( XTalentInfo* pTalentInfo, const vec3& vPos, const vec3& vDir )
{
	m_pOwner->SetPosition( vPos);
	m_pOwner->SetDirection( vDir);

	XMovableGoParam* pGoParam = new XMovableGoParam(XMovableGoParam::E_GO_INTERPOLATION);
	XMovableFactor::SInterpolation param = m_pModuleMovable->GetMovableFactor().INTERPOLATION;
	pGoParam->Set( vPos, vDir, param.m_fMoveSpeed, param.m_fRotateSpeed, BOOST_NONE, MOVEMENTFLAG_FORWARD, true);
	pGoParam->SetGoInterpolationForce(true);
	m_pModuleMovable->TriggerGo( pGoParam);
	delete pGoParam;

	m_pModuleTalent->ExtraAct();
}

void XModuleActorControl::OnUpdate( float fDelta )
{
	PFC_THISFUNC;

	if (OnUpdateCheckUpdatable())
	{
		UpdateDuelGestureChecker();

		OnUpdateState(fDelta);
		OnUpdateCommand(fDelta);
		OnUpdateSpecial(fDelta);

		m_DuelFightChecker.Update();

		if(m_pOverlappedEquipmentController)
			m_pOverlappedEquipmentController->OnUpdate(m_pOwner);
	}
}

void XModuleActorControl::OnEnterGame( const vec3& vPos, const vec3& vDir, const XActor* pCallerOwnerActor )
{
	_ASSERT(pCallerOwnerActor == m_pOwner);
	OnWarp(vPos, vDir);
}

void XModuleActorControl::OnBallonText( wstring szID, wstring szChat, float fMaintainTime )
{
	XModuleUI* pModuleUI = m_pOwner->GetModuleUI();
	if ( pModuleUI == NULL) return;

	wstring strTransChat;
	XStringResMgr::GetInstance()->TransKeyStr(strTransChat, szChat.c_str());

	MUID uidOwner = m_pOwner->GetUID();
	mmcode::MControlCodeTransResult result = XMMCodeMgr::GetInstance().TransControlCodeAndReplaceChars(strTransChat.c_str(), XGetMyUID());
	result.Proc(&uidOwner);
	strTransChat = MLocale::ConvAnsiToUTF16(result.m_strOutput.c_str());

	if (strTransChat.empty() == false)
	{
		pModuleUI->OnBallonText(szID, strTransChat, fMaintainTime);
	}
}


void XModuleActorControl::ShowHitAndDamage( _DamageInfo * pInfo, XObject * pAttacker, const vec3& vOffset )
{
	m_pDamageInfo = NULL;
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(pInfo->nAttackedTalentID, pAttacker);
	if(pTalentInfo)
	{
		if(pTalentInfo->m_nSkillType == ST_MAGIC &&
			pTalentInfo->m_Events.HasEvent(TE_LAUNCH_PROJECTILE))
		{
			// 마법 발사체이다.
			XProjectileController projectileController;
			projectileController.ProjectileHit(m_pOwner, pAttacker, pTalentInfo);
		}
	}

	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		// 이펙트 보여줄때 사용하기 위한 정보이다.
		m_pDamageInfo = pInfo;

		XHitDamageEffectEventData * pEventData = new XHitDamageEffectEventData;
		pEventData->m_Info			= *pInfo;
		if(pAttacker)	pEventData->m_uidAttacker	= pAttacker->GetUID();
		pEventData->m_vOffset		= vOffset;
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_SHOW_HIT_DAMAGE, pEventData);

		// https://martini.neowiz.com/issues/619 이 문제로 여기서 바로 표시
		pModuleEffect->ShowDamageInfo(pInfo, m_pOwner->GetUID()); 
	}

	m_pDamageInfo = NULL;
}

_DamageRepeatInfo * XModuleActorControl::InitDamageRepeat( XTalentHitParam * pInfo )
{
	if(pInfo)
	{
		// 정보 셋팅
		_DamageRepeatInfo * pDamageInfo = new _DamageRepeatInfo;

		// 기본 히트 정보
		pDamageInfo->nTalentID		= pInfo->nTalentID;
		pDamageInfo->nDamage				= pInfo->nDamage;
		pDamageInfo->nFlags					= pInfo->nFlags;
		pDamageInfo->nMFState				= pInfo->nMFState;
		pDamageInfo->nMFWeight				= pInfo->nMFWeight;
		pDamageInfo->nAttackerUIID			= pInfo->nAttackerUIID;
		pDamageInfo->vServerKnockbackDir	= pInfo->vServerKnockbackDir;

		// 연타 정보
		XObject* pAttackObject = gg.omgr->FindActor_UIID(pInfo->nAttackerUIID);
		if(pAttackObject)
		{
			XModuleTalent * pModuleTalent = pAttackObject->GetModuleTalent();
			if(pModuleTalent && pModuleTalent->GetTalent())
			{
				XTalentEvent* pTalentEvent = pModuleTalent->GetTalent()->GetTalentEvent();
				if(pTalentEvent)
				{
					vector<CSTalentEventInfo> vecDamageEvent;
					pTalentEvent->GetActTalentEvent(TE_DAMAGE, vecDamageEvent);

					// 데이터
					float fActElapsedTime = 0.0f;
					
					if(pModuleTalent->GetTalent()->GetAllowedActingFromServer())
						fActElapsedTime = pModuleTalent->GetTalent()->GetActElapsedTime();

					for(vector<CSTalentEventInfo>::iterator it = vecDamageEvent.begin(); it != vecDamageEvent.end(); it++)
					{
						_DamageRepeatEvent stDamageRepeatEvent;

						stDamageRepeatEvent.fTime			= (*it).m_fTime1 - fActElapsedTime;
						stDamageRepeatEvent.strEffectName	= (*it).m_strParam1;
						stDamageRepeatEvent.strBone			= (*it).m_strParam2;

						// 데미지 계산
						if(m_pOwner->GetUID() == XGetMyUID())
							stDamageRepeatEvent.nDamage = (int)( (pDamageInfo->nDamage * (*it).m_nParam1) / 100.0f);

						pDamageInfo->vecDamageRepeatEvents.push_back(stDamageRepeatEvent);
					}
				}
			}
		}

		return pDamageInfo;
	}

	return NULL;
}

void XModuleActorControl::CheckSwim( bool bIn, bool bFallingStart /*= false*/ )
{
	// 플레이어만 수영이 가능하다.
	if(m_pOwner->GetEntityType() != ETID_PLAYER)
	{
		return;
	}

	// 맵 로딩이 완료가 안되었는데... 설정이 되면 안된다.
	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if(pModuleCollision && pModuleCollision->CheckTerrainCollisionUsable() == false)
	{
		if(bIn == true)
			m_eReservedSwimState = SS_DOING_SWIM;
		else 
			m_eReservedSwimState = SS_NOT_SWIM;

		return;
	}

	m_eReservedSwimState = SS_NONE;

	XPlayer* pPlayer = static_cast<XPlayer*>(m_pOwner);
	bool bSwim = pPlayer->IsSwim();
	if(bIn)
	{
		// 무기를 일반으로 변경한다.
		if(pPlayer->IsCurrStanceBattle())
		{
			Stop();

			XEvent evt(XEVTL_CHANGE_STANCE);
			m_pOwner->PostDelayEvent(evt, 0);
		}

		pPlayer->SetSwim(true);

		// 지상에서 물속으로 첫 진입시...
		if(bSwim == false)
		{
			m_pModuleAction->Swim();

			// 이펙트
			// 떨어졌을때에만 처리
			if(bFallingStart)
				global.ui->OnSwimStartEffect(m_pOwner->GetPosition());
		}
	}
	else
	{
		pPlayer->SetSwim(false);

		// 물속에서 지상으로 첫 진입시...
		if(bSwim == true)
		{
			Stop();

			// 이펙트
		}
	}
}

void XModuleActorControl::OnTalentInstantEffectGain( XTalentInfo* pTalentInfo )
{
	// 이펙트
	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		XTalentEffectEventData* pCastEffectData = new XTalentEffectEventData;
		pCastEffectData->Set(pTalentInfo, ATTACK_EFFECT, m_pOwner, vec3(0.0f, 0.0f, 0.0f), NULL);
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_TALENT, pCastEffectData);
	}
}

void XModuleActorControl::OnFallingDamage( int nDamage )
{
	// 피격및 데미지 보여주기
	_DamageInfo damageInfo;
	damageInfo.nDamage				= nDamage;
	damageInfo.nFlags				= 0;
	SetBitSet(damageInfo.nFlags, CTR_FALLING);

	ShowHitAndDamage(&damageInfo, m_pOwner);
}

void XModuleActorControl::BirdTest_ShowDelay( XTalentDelayParam * pHitInfo )
{
	return;
	XObject* pAttacker = gg.omgr->FindActor_UIID(pHitInfo->nAttackerUIID);

	if (pAttacker->GetModuleTalent()->IsActive())
	{
		XBaseTalent* pCurrTalent = pAttacker->GetModuleTalent()->GetTalent();
		if (pCurrTalent)
		{
			float fTalentElapsedTime = pCurrTalent->GetElapsedTime();
			wchar_t text[256];			
	
			uint32 nNowTime = global.system->GetTimer().GetNowGlobalTime();
			uint32 nTalentStartTime = nNowTime - (uint32)(fTalentElapsedTime * 1000);

			float fRecvElapsedTime = (pHitInfo->GetRecvTime() - nTalentStartTime) / 1000.0f;


			swprintf_s(text, L"%.02f:%.02f", fRecvElapsedTime, fTalentElapsedTime);

			if(pAttacker->GetModuleEffect())
			{
				XCaptionEffectEventData * pEventData = new XCaptionEffectEventData;
				pEventData->m_strMessage = text;
				pEventData->m_color = 0xFFFF00FF;
				pAttacker->GetModuleEffect()->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_DEBUG, pEventData);
			}
		}
	}
}

bool XModuleActorControl::IsCurrentMotionGuardKnockback()
{
	if(m_pModuleAction == NULL)
		return  false;

	wstring strMotionName = m_pModuleAction->GetCurrentMotionName();
	if(strMotionName == MOTION_NAME_GUARD_KNOCKBACK ||
		strMotionName == MOTION_NAME_SHIELD_GUARD_KNOCKBACK)
		return true;

	return false;
}

void XModuleActorControl::OnGuardKnockBack(vec3& vDir)
{
	if(m_pModuleAction == NULL)
		return;

	m_pOwner->SetDirection(vDir);

	m_pModuleAction->GuardKnockBack();
}

void XModuleActorControl::OnHitByBuff( TD_BUFF_HIT* pBuffHitInfo )
{
	// 현재 사용중인 탤런트 삭제
	// MyControl은 제외 MyAction에서 하고 있다.
	if(XGetMyUID() != m_pOwner->GetUID())
	{
		if (m_pModuleTalent->IsActive())
		{
			if ( m_pModuleTalent->GetInfo()->m_bCancelable && 
				XTalentHelper::IsTalentCancelableMotionFactor((MF_STATE)pBuffHitInfo->nMFState) )
			{
				// 사용중이던 탈랜트 사용중지
				m_pModuleTalent->Cancel();
			}
		}
	}

	// 모션 팩터
	MUID uidEmpty;

	XTalentHitParam HitParam;
	HitParam.nMFState	= (MF_STATE)pBuffHitInfo->nMFState;
	HitParam.nMFWeight	= pBuffHitInfo->nMFWeight;

	HitByBuff(&HitParam);
}

void XModuleActorControl::OnGuarded()
{
	if(m_pModuleAction == NULL)
		return;

	bool bKeepGuard = IsCurrentGuard();
	m_pModuleAction->Guarded(bKeepGuard);
}

void XModuleActorControl::OnDieAniEnd()
{
	// 죽은 애니메이션 다음에 처리해야 하는 것들
	// 버프 관련 처리
	m_pModuleBuff->BuffDie();

	XEvent msg(XEVTL_DIE_ANI_END);
	m_pOwner->Event(msg);
}

void XModuleActorControl::CheckReservedHitByTalent()
{
	--m_nReservedHitByTalentCount;
	if(m_nReservedHitByTalentCount <= 0)
	{
		m_nReservedHitByTalentCount = 0;
		m_bReservedHitByTalent = false;
	}
}

void XModuleActorControl::OnGuardFailed()
{
	OnGuardReleased();
}

void XModuleActorControl::UpdateHitDamage( int nDamage )
{
	if(m_nApplicationDamageHpByTalentCount > 0)
	{
		--m_nApplicationDamageHpByTalentCount;
		return;
	}

	m_nApplicationDamageHpByTalentCount = 0;

	//////////////////////////////////////////////////////////////////////////
	// 준비
	XActor* pActor = static_cast<XActor*>(m_pOwner);
	TD_ENEMY_INFO* pEnemyInfo = gvar.Game.EnemyInfo.GetEnemyInfoByUIID(pActor->GetUIID());

	if(pEnemyInfo == NULL)
		return;

	//////////////////////////////////////////////////////////////////////////
	// 죽었다면... 0으로 만들기 위해 데미지를 max만큼 줘버리자.
	if(pActor->IsDead())
		nDamage = pEnemyInfo->nMaxHP;

	//////////////////////////////////////////////////////////////////////////
	// 계산
	int nCurHpPercent = pEnemyInfo->nHPPercent;
	int nDownPercent = (nDamage * 100.0f) / pEnemyInfo->nMaxHP;
	nCurHpPercent -= nDownPercent;
	if(nCurHpPercent < 0)
		nCurHpPercent = 0;

	pEnemyInfo->nHPPercent = nCurHpPercent;
}

void XModuleActorControl::SetApplicationDamage()
{
	m_nApplicationDamageHpByTalentCount = m_nReservedHitByTalentCount;
}

void XModuleActorControl::OnReInField( vec3 vPos, vec3 vDir )
{
	// 만약 삭제 요청이 있었다면 삭제 취소
	gg.omgr->CancelReserveDelete(m_pOwner->GetUID());

	if (m_pModuleEntity->IsFading())
	{
		m_pModuleEntity->StopFade();
	}

	// 만약 안보이는 상태라면 다시 보이게 함
	m_pModuleEntity->SetVisibility(1.0f);

	// 위치도 새로 갱신함
	m_pOwner->SetPosition(vPos);

	m_pOwner->SetDirection(vDir);

}

void XModuleActorControl::OnActGoundTarget( int nTalentID, vec3& vEntityDir, vec3& vTargetPos )
{
	// 타겟 설정
	m_pModuleTalent->SetTarget(MUID::ZERO, -1, -1, vTargetPos);

	OnActTalent(nTalentID, vEntityDir);
}

void XModuleActorControl::OnEffectLevelUP()
{
	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, wstring(EFFECT_NAME_LEVEL_UP), wstring(), wstring());
}

void XModuleActorControl::OnEffectCreateGuild()
{
	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, wstring(EFFECT_NAME_CREATEGUILD), wstring(), wstring());
}

void XModuleActorControl::OnEffectDyeDone()
{
	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, wstring(EFFECT_NAME_DYE_DONE), wstring(), wstring());
}

void XModuleActorControl::OnEffectFatigueBest()
{
	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, wstring(EFFECT_NAME_FATIGUE_BEST), wstring(), wstring());
}

void XModuleActorControl::OnUpdateState( float fDelta )
{
	if(m_eReservedSwimState != SS_NONE)
	{
		if(m_eReservedSwimState == SS_DOING_SWIM)
			CheckSwim(true);
		else
			CheckSwim(false);
	}
}

void XModuleActorControl::OnHeal( int nAmount, UIID nUserUIID /*= UIID_INVALID*/ )
{
	// 힐 캡션은 효과받는 나만 보인다.
	// 힐 캡션은 효과준 사람이 나라면 보인다.
	if(m_pOwner->GetUID() != XGetMyUID() && 
		(nUserUIID == UIID_INVALID || nUserUIID != XGetMyUIID()))
		return;

	// 힐 캡션 효과 처리(공통사항)
	XCaptionEffectEventData* pEventData = new XCaptionEffectEventData;
	pEventData->m_nParam1 = nAmount;
	pEventData->m_uiidUser = nUserUIID;

	XModuleEffect * pModuleEffect = m_pOwner->GetModuleEffect();
	if(pModuleEffect)
	{
		pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_CAPTION_HEAL, pEventData);
	}
}

void XModuleActorControl::OnTalentHeal( UIID nUserUIID, int nTalentID, int nAmount )
{
	OnHeal(nAmount, nUserUIID);

	XTalentInfo* pInfo = info.talent->Get(nTalentID);
	if (pInfo == NULL)	return;

	OnTalentInstantEffectGain(pInfo);
}

void XModuleActorControl::OnBuffHeal( UIID nUserUIID, int nBuffID, int nAmount )
{
	OnHeal(nAmount, nUserUIID);

	XModuleEffect * pEffect = m_pOwner->GetModuleEffect();
	if(pEffect)
	{
		XBuffEffectEventData* pEventData = new XBuffEffectEventData;
		pEventData->m_nParam1			= nBuffID;
		pEventData->m_eEffectType		= BUFF_HIT_EFFECT;
		pEffect->OnEffectEvent(XEFTEVT_EFFECT_BUFF, pEventData);
	}
}


void XModuleActorControl::FakeBeatenAndSetMultiplyColor( XTalentHitParam * pHitInfo, bool bForceFakeBeaten )
{
	if (XCONST::USE_HIT_EFFECT_MODEL_ADD_COLOR)
	{
		//// 파츠 히트시 사용 안함
		//if (CheckBitSet(pHitInfo->nFlags, CTR_HIT_BPART1) || 
		//	CheckBitSet(pHitInfo->nFlags, CTR_HIT_BPART2) ||
		//	CheckBitSet(pHitInfo->nFlags, CTR_HIT_BPART3) ||
		//	CheckBitSet(pHitInfo->nFlags, CTR_HIT_BPART4))
		//	return;

		m_pModuleEntity->StartMultiplyColor();
	}

	//////////////////////////////////////////////////////////////////////////
	// 리액션
	// 회피는 리액션 없음
	// 죽었으면.. .리액션 없음.
	XActor* pActor = AsActor(m_pOwner);
	if (pHitInfo->IsFlagAvoid() ||
		pActor->IsDead()) 
		return;

	if (pHitInfo->nMFState == MF_NONE || pHitInfo->nMFState == MF_FAKE_BEATEN || bForceFakeBeaten == true)
	{
		WEAPON_TYPE nAttackWeaponType = WEAPON_NONE;
		XObject* pAttackObject = gg.omgr->FindActor_UIID(pHitInfo->nAttackerUIID);
		if(pAttackObject && pAttackObject->GetEntityType() == ETID_PLAYER)
		{
			nAttackWeaponType = pAttackObject->AsPlayer()->GetCurrWeaponType();
		}

		float fCriWeight = 1.0f;
		float fCriSpeed = 1.0f;

		if ( pHitInfo->IsFlagCritical())
		{
			fCriWeight = XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_WEIGHT;
			fCriSpeed = XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_CIRITICAL_SPEED;
		}

		m_pModuleEntity->StartFakeBeaten(nAttackWeaponType, fCriWeight, fCriSpeed);
	}
}

void XModuleActorControl::OnBuffIncrease( int nBuffID, float fLostRemained )
{
	m_pModuleBuff->BuffIncrease(nBuffID, fLostRemained);
}

void XModuleActorControl::OnBuffDecrease( int nBuffID )
{
	m_pModuleBuff->BuffDecrease(nBuffID);
}

void XModuleActorControl::DoActionDuelGesture( const wchar_t* szMotionName )
{
	// 아이들 상태인가 검사...
	wstring strCurMotion = m_pOwner->GetModuleMotion()->GetCurrMotion();
	if(strCurMotion == MOTION_NAME_IDLE && IsReservedHitByTalent() == false)
	{
		DoActionGesture(szMotionName);
	}
	else
	{
		// 아이들이 아니면 아이들이 될때 제스쳐를 보여준다.
		m_DuelGestureChecker.SetGesture(szMotionName);
	}
}

void XModuleActorControl::UpdateDuelGestureChecker()
{
	if(m_DuelGestureChecker.CheckUsableGesture(m_pOwner) == true)
	{
		DoActionDuelGesture(m_DuelGestureChecker.GetGesture());
		m_DuelGestureChecker.Init();
	}
}

void XModuleActorControl::StartOverlappedEquipmentController(int nItemID)
{
	if(m_pOwner->GetEntityType() != ETID_PLAYER)
		return;

	m_pOverlappedEquipmentController->OnStart(m_pOwner->AsPlayer(), nItemID);
}

void XModuleActorControl::EndOverlappedEquipmentController(int nItemID)
{
	m_pOverlappedEquipmentController->OnEnd(m_pOwner, nItemID);
}
