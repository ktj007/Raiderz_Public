#include "StdAfx.h"
#include "XMagicArea.h"
#include "XModuleEntity.h"
#include "XModuleBuff.h"
#include "XEffect.h"
#include "XSystem.h"
#include "XTalentInfoMgr.h"
#include "XNPCInfo.h"
#include "XGameTransformControllerManager.h"
#include "XBuffInfo.h"

MImplementRTTI(XMagicArea, XObject);

// 마법진에 쓰이는 애니메이션은 정적입니다.
#define MAGICAREA_IDLE		L"idle"
#define MAGICAREA_DESPAWN	L"die"
#define MAGICAREA_SPAWN		L"MF_KD3"
#define MAGICAREA_ATTACK	L"spell_fire"

XMagicArea::XMagicArea(MUID uid)
: XObject(uid)
, m_pNPCInfo(NULL)
, m_bModelLoadingComplete(false)
, m_bDespawn(false)
, m_bStarted(false)
, m_fRemainTime(0.0f)
, m_bUseRemainTime(false)
{
	m_nTypeID			= ETID_BUFF_ENTITY;

	m_pModuleEntity		= new XModuleEntity(this);
}

XMagicArea::~XMagicArea()
{
	SAFE_DELETE(m_pModuleEntity);
}

void XMagicArea::RegisterModules()
{
	XObject::RegisterModules();

	// Initialize 먼저
	if(m_pModuleEntity)
	{
		m_pModuleEntity->Initialize();
		m_Modules.AddModule(m_pModuleEntity, true);
	}
}

void XMagicArea::UnregisterModules()
{
	// Remove 먼저..
	if(m_pModuleEntity)
	{
		m_Modules.RemoveModule(m_pModuleEntity);
		m_pModuleEntity->Finalize();
	}

	XObject::UnregisterModules();
}

void XMagicArea::OnUpdate( float fDelta )
{
	XObject::OnUpdate(fDelta);
	CheckRemainTime(fDelta);
	m_Effect.Update(fDelta);

	// 모델 로딩 이후에 해야하는 것들...
	if(CheckModelLoadingComplete())
		DoModelLoadingComplete();
	
	// 애니메이션 상태에 따른 설정
	CheckAnimationState();

	// 디버그
	CheckDebugMagicArea();
}

bool XMagicArea::OnCreate()
{
	return true;
}

void XMagicArea::OnDestroy()
{
	m_Effect.Destroy();
}

void XMagicArea::OnRender()
{
}

void XMagicArea::Setup( vec3& vPos, int nBuffID, float fReMainTime, bool bStarted /*= false*/ )
{
	XBuffInfo* pBuffInfo = info.buff->Get(nBuffID);
	if(pBuffInfo == NULL)
		return;

	SetVisible(true);

	// 정보
	SetPosition(vPos);
	m_fRemainTime = fReMainTime;

	// 주의!!!! 남는 시간이 제로는 영원히 존재하는 것이다. 그렇게 얘기가 되었다!!!!!!!
	if(m_fRemainTime > 0.0f)
		m_bUseRemainTime = true;

	// 이펙트
	m_Effect.Create(this, pBuffInfo, vPos, GetDirection());

	// 모델
	SetupModel(pBuffInfo);

	// 이미 시작된 마법진인가?
	m_bStarted = bStarted;
}

void XMagicArea::SetupModel( XBuffInfo* pBuffInfo )
{
	int nNPCID = pBuffInfo->m_nExtraActiveModelID;
	if (nNPCID > 0)
	{
		XNPCInfo* pNPCInfo = info.npc->Get(nNPCID);
		if(pNPCInfo == NULL)
			return;

		// npc
		m_pNPCInfo = pNPCInfo;

		if (m_pModuleEntity)
		{
			m_pModuleEntity->SetScale(m_pNPCInfo->fScale);
			m_pModuleEntity->SerializeNPC(pNPCInfo);
		}
	}
}

void XMagicArea::AniSpawn()
{
	m_pModuleEntity->GetActor()->SetAnimation(MLocale::ConvUTF16ToAnsi(MAGICAREA_SPAWN).c_str());
}

void XMagicArea::AniIdle()
{
	m_pModuleEntity->GetActor()->SetAnimation(MLocale::ConvUTF16ToAnsi(MAGICAREA_IDLE).c_str());
}

void XMagicArea::AniAttack()
{
	if(m_pModuleEntity->GetActor()->SetAnimation(MLocale::ConvUTF16ToAnsi(MAGICAREA_ATTACK).c_str()) == false)
	{
		DeleteMe();
	}
}

void XMagicArea::AniDespawn()
{
	if(m_pModuleEntity->GetActor()->SetAnimation(MLocale::ConvUTF16ToAnsi(MAGICAREA_DESPAWN).c_str()) == false)
	{
		DeleteMe();
	}

	m_bDespawn = true;
}

void XMagicArea::TrapAction()
{
	AniAttack();
}

void XMagicArea::Despawn()
{
	AniDespawn();
}

bool XMagicArea::CheckModelLoadingComplete()
{
	if(m_bModelLoadingComplete == false &&
		m_pModuleEntity->IsLoadingCompleted())
	{
		m_bModelLoadingComplete = true;

		return true;
	}

	return false;
}

void XMagicArea::CheckAnimationState()
{
	// 애니메이션 상태에 따른 설정
	if(m_pModuleEntity)
	{
		// 1. 스폰이후에는 Idle 애니메이션으로 변경한다.
		if(m_pModuleEntity->GetAnimationController()->GetAnimationName() == MAGICAREA_SPAWN &&
			m_pModuleEntity->GetAnimationController()->IsPlayDone())
			AniIdle();

		// 2. 공격이나 사라지는 경우 이후에는 자동으로 지우게 한다.
		if((m_pModuleEntity->GetAnimationController()->GetAnimationName() == MAGICAREA_DESPAWN ||
			m_pModuleEntity->GetAnimationController()->GetAnimationName() == MAGICAREA_ATTACK) &&
			m_pModuleEntity->GetAnimationController()->IsPlayDone())
			DeleteMe();
	}
}

void XMagicArea::DoModelLoadingComplete()
{
	// 로딩 완료 후 행하는 것들...
	// 죽었나?
	if(m_bDespawn)
		return;

	// 이미 시작된거라면...
	if(m_bStarted)
	{
		AniIdle();
		return;
	}

	// 스폰
	AniSpawn();
}

void XMagicArea::CheckDebugMagicArea()
{
	m_Effect.SetDebugMagicArea(gvar.Debug.bShowMagicArea);
}

void XMagicArea::CheckRemainTime( float fDelta )
{
	if(m_bUseRemainTime == false)
		return;

	m_fRemainTime -= fDelta;
	if(m_fRemainTime <= 0.0f && m_bDespawn == false)
	{
		// 끝났다.
		Despawn();
	}
	else
	{
		m_Effect.CheckRemainTimeEffect(m_fRemainTime);
	}
}