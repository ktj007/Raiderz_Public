#include "stdafx.h"
#include "XActor.h"
#include "XConst.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleMovable.h"
#include "XModuleAction.h"
#include "XModuleCollision.h"
#include "XModuleTalent.h"
#include "XModuleBackgroundWork.h"
#include "XItem.h"
#include "XItemContainer.h"
#include "XEquipmentSlot.h"
#include "XModuleMessageQueue.h"
#include "XModuleEffect.h"
#include "XModuleBuff.h"
#include "XModuleUI.h"
#include "XGameTransformControllerManager.h"
#include "RMesh.h"
#include "CSCharHelper.h"

bool IsActorDead(XObject* pObject)
{
	if ((pObject->GetEntityType() == ETID_PLAYER) || (pObject->GetEntityType() == ETID_NPC))
	{
		XActor* pTargetActor = static_cast<XActor*>(pObject);
		return pTargetActor->IsDead();
	}
	return false;
}

bool IsActorColliable(XObject* pObject)
{
	if ((pObject->GetEntityType() == ETID_PLAYER) || (pObject->GetEntityType() == ETID_NPC))
	{
		XActor* pTargetActor = static_cast<XActor*>(pObject);
		return pTargetActor->IsColliable();
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////

MImplementRTTI(XActor, XObject);

XActor::XActor(MUID uid, UIID nUIID) : XObject(uid), m_bDead(false), m_nUIID(nUIID)
{
	m_fSTARegenTime = 0.0f;
	m_fSTARegenModValue = 0.0f;
	m_nDeadTime = 0;
	m_bLoadingAsync = true;
	m_bGuard = false;

	m_pModuleEntity			= new XModuleEntity(this);
	m_pModuleMotion			= new XModuleMotion(this);
	m_pModuleAction			= new XModuleAction(this);
	m_pModuleMovable		= new XModuleMovable(this);
	m_pModuleCollision		= new XModuleCollision(this);
	m_pModuleTalent			= new XModuleTalent(this);
	m_pModuleMessageQueue	= new XModuleMessageQueue(this);
	m_pModuleEffect			= new XModuleEffect(this);
	m_pModuleBuff			= new XModuleBuff(this);
	m_pModuleUI				= new XModuleUI(this);
	m_pModuleBackgroundWork = new XModuleBackgroundWork(this);
}

XActor::~XActor()
{
	SAFE_DELETE(m_pModuleMessageQueue);
	SAFE_DELETE(m_pModuleTalent);
	SAFE_DELETE(m_pModuleMovable);
	SAFE_DELETE(m_pModuleAction);
	SAFE_DELETE(m_pModuleMotion);
	SAFE_DELETE(m_pModuleCollision);
	SAFE_DELETE(m_pModuleEntity);
	SAFE_DELETE(m_pModuleEffect);
	SAFE_DELETE(m_pModuleBuff);
	SAFE_DELETE(m_pModuleUI);
	SAFE_DELETE(m_pModuleBackgroundWork);
}

void XActor::RegisterModules()
{
	XObject::RegisterModules();

	// Initialize 먼저
	m_pModuleEntity->Initialize();
	m_Modules.AddModule(m_pModuleEntity, true);

	m_pModuleMotion->Initialize();
	m_Modules.AddModule(m_pModuleMotion, true);

	m_pModuleAction->Initialize();
	m_Modules.AddModule(m_pModuleAction, true);

	m_pModuleMovable->Initialize();
	m_Modules.AddModule(m_pModuleMovable, true);

	m_pModuleCollision->Initialize();
	m_Modules.AddModule(m_pModuleCollision, true);

	m_pModuleTalent->Initialize();
	m_Modules.AddModule(m_pModuleTalent, true);

	m_pModuleMessageQueue->Initialize();
	m_Modules.AddModule(m_pModuleMessageQueue, true);

	m_pModuleEffect->Initialize();
	m_Modules.AddModule(m_pModuleEffect, true);

	m_pModuleBuff->Initialize();
	m_Modules.AddModule(m_pModuleBuff, true);

	m_pModuleUI->Initialize();
	m_Modules.AddModule(m_pModuleUI, true);

	m_pModuleBackgroundWork->Initialize();
	m_Modules.AddModule(m_pModuleBackgroundWork, true);
}

void XActor::UnregisterModules()
{
	// Remove 먼저..
	m_Modules.RemoveModule(m_pModuleBackgroundWork);
	m_pModuleBackgroundWork->Finalize();

	m_Modules.RemoveModule(m_pModuleUI);
	m_pModuleUI->Finalize();

	m_Modules.RemoveModule(m_pModuleBuff);
	m_pModuleBuff->Finalize();

	m_Modules.RemoveModule(m_pModuleEffect);
	m_pModuleEffect->Finalize();

	m_Modules.RemoveModule(m_pModuleMessageQueue);
	m_pModuleMessageQueue->Finalize();

	m_Modules.RemoveModule(m_pModuleTalent);
	m_pModuleTalent->Finalize();

	m_Modules.RemoveModule(m_pModuleMovable);
	m_pModuleMovable->Finalize();

	m_Modules.RemoveModule(m_pModuleCollision);
	m_pModuleCollision->Finalize();

	m_Modules.RemoveModule(m_pModuleAction);
	m_pModuleAction->Finalize();

	m_Modules.RemoveModule(m_pModuleMotion);
	m_pModuleMotion->Finalize();

	m_Modules.RemoveModule(m_pModuleEntity);
	m_pModuleEntity->Finalize();

	XObject::UnregisterModules();
}


void XActor::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	XObject::OnUpdate(fDelta);
}

void XActor::OnRender()
{
	XObject::OnRender();

	if (XScreenDebugger::GetInstance()->IsVisible())
	{
		_RenderDebug();
	}
}

void XActor::_RenderDebug()
{
	// [3/26/2007 madduck] - 현재모션이름그리기.
	vec3 vTextPos = GetPosition();
	vTextPos.z += 150.0f+5.0f;

	if (GetModuleEntity()->GetActor() ||
		GetModuleEntity()->GetActor()->m_pMesh == NULL)
		return;

	wstring n = MLocale::ConvAnsiToUTF16(GetModuleEntity()->GetActor()->m_pMesh->GetName().c_str());
	wstring nn = n.substr(n.rfind(L"/") + 1, n.rfind(L".") - n.rfind(L"/") - 1);

	if(GetModuleMotion() != NULL)
	{
		wstring m = GetModuleMotion()->GetCurrMotion();
		XRenderHelper::DrawText(vTextPos, MCOLOR(0x66aaaaff), m.c_str());
	}
}

void XActor::OnDie()
{ 
	m_bDead = true; 
	m_nDeadTime = XGetNowTime(); 

	// 죽었다면 페이스 Blink 애니메이션 안한다.
	if(m_pModuleEntity)
		m_pModuleEntity->SetFaceBlinkAnimationActive(false);

	if(m_pModuleMovable)
		m_pModuleMovable->TriggerDie(true);
}

void XActor::OnRebirth()
{ 
	m_bDead = false; 
	m_nDeadTime = 0;

	m_pModuleEntity->InitRenderPosDirUp();

	// 살았다면 페이스 Blink 애니메이션 한다.
	if(m_pModuleEntity)
		m_pModuleEntity->SetFaceBlinkAnimationActive(true);
}

bool XActor::IsColliable()
{
	if (IsDead())
	{
		const unsigned int DEAD_COLLISION_FREE_TIME = 500;
		unsigned int nNowTime = XGetNowTime();
		if (nNowTime - m_nDeadTime > DEAD_COLLISION_FREE_TIME) return false;
	}

	return true;
}

void XActor::AddToSceneManager()
{
	if (m_pModuleEntity) m_pModuleEntity->AddToSceneManager();
}

bool XActor::RemoveFromSceneManager()
{
	if (m_pModuleEntity) return m_pModuleEntity->RemoveFromSceneManager();
	return false;
}

float XActor::GetRadius()
{
	return m_pModuleCollision->GetRadius();
}

void XActor::DeadProcEnterField()
{
	if(m_bDead)
	{
		// 죽은거면 죽은 애니메이션 처리
		if(m_pModuleMotion && m_pModuleAction && m_pModuleEntity)
		{
			wstring strMotionName = m_pModuleMotion->GetCurrMotion();
			if(strMotionName != MOTION_NAME_NPC_DIE &&
				strMotionName != MOTION_NAME_DIE)
			{
				m_pModuleAction->Die();
			}

			RActor* pAniActor = m_pModuleEntity->GetActor();
			if(pAniActor)
			{
				// 죽은 애니메이션을 마지막으로 셋팅
				m_pModuleEntity->GetAnimationController()->SetAnimationFrame(m_pModuleEntity->GetAnimationController()->GetAniMaxFrame());

				// 이벤트 발생을 하지 않는다.
				m_pModuleEntity->GetAnimationController()->CloseAnimationEvent();

				// 지형 경사에 따라 Up 설정
				m_pModuleAction->DoneDIe();

				// 알파 테스트 이펙트가 있는가?
				CheckAlphaTestFadeEffect();
			}
		}
	}
}

wstring XActor::GetCurrentAnimationName()
{
	return m_pModuleEntity->GetAnimationController()->GetAnimationName();
}

void XActor::SetOpenHitGroupID( int nGroupID )
{
	if(m_pModuleCollision)
	{
		m_pModuleCollision->SetHitCapsuleByTrigger(nGroupID);
	}
}

float XActor::GetValidInteractionDistance()
{
	return CSCharHelper::CalcValidInteractionDistance(m_pModuleCollision->GetRadius());
}

void XActor::OnPostRender()
{
	m_pModuleEntity->ReSetAnimationUpdateCompleted();
}

bool XActor::IsCurrentAnimationAlphaTestFadeEffect()
{
	vector<ANIMATION_EVENT>* pAnimationEvent = m_pModuleEntity->GetAnimationController()->GetAnimationEvent();
	if(pAnimationEvent == NULL)
		return false;

	for(vector<ANIMATION_EVENT>::iterator it = pAnimationEvent->begin(); it != pAnimationEvent->end(); ++it)
	{
		if(it->pAnimationEvent->m_strEvent == "sh_custom")
		{
			XEffectInfo* pEffectInfo = info.effect_Info->GetEffectInfo(MLocale::ConvAnsiToUTF16(it->pAnimationEvent->m_strParam1.c_str()));
			if(pEffectInfo)
			{
				for(vector<XBaseEffectInfoItem *>::iterator itEffect = pEffectInfo->m_vecEffectInfoItem.begin(); itEffect != pEffectInfo->m_vecEffectInfoItem.end(); ++itEffect)
				{
					if((*itEffect)->eEffectType == CET_ALPHATESTFADE)
						return true;
				}
			}
		}
	}

	return false;
}

void XActor::CheckAlphaTestFadeEffect()
{
	if(IsCurrentAnimationAlphaTestFadeEffect())
	{
		GetActor()->SetNoiseRef(0.0f);
	}
}