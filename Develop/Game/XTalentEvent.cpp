#include "stdafx.h"
#include "XTalentEvent.h"
#include "XCameraEffect.h"
#include "XTalent.h"
#include "XCameraManager.h"
#include "XEffectInvoker.h"
#include "XCharacter.h"
#include "XTalentHelper.h"
#include "XModuleMyControl.h"

//////////////////////////////////////////////////////////////////////////
XTalentEvent::XTalentEvent( XBaseTalent* pTalent, XObject* pOwner /*= NULL*/ )
: XBaseTalentEvent()
{
	m_pOwner						= pOwner;
	m_pTalent						= pTalent;
}

XTalentEvent::~XTalentEvent()
{
	m_pOwner						= NULL;
	m_pTalent						= NULL;
}

void XTalentEvent::EventUpdate( float fDelta )
{
	if(IsCurrentTalent() == false)
	{
		return;
	}

	XBaseTalentEvent::EventUpdate(fDelta);
}

bool XTalentEvent::IsCurrentTalent()
{
	if(m_pOwner)
	{
		if(m_pTalent &&
			m_pTalent->IsActive() == false)
		{
			EventsEnd();
			return false;
		}
	}

	return true;
}

void XTalentEvent::GetActTalentEvent( TALENT_EVENT eventtype, vector<CSTalentEventInfo>& outVecEvents )
{
	if(m_pCurrentTalentEvent)
	{
		for(vector<CSTalentEventInfo>::iterator itAct = m_pCurrentTalentEvent->m_vecActEvents.begin(); itAct != m_pCurrentTalentEvent->m_vecActEvents.end(); ++itAct)
		{
			CSTalentEventInfo * pEvent = &(*itAct);

			if(pEvent->m_nEvent == eventtype)
			{
				outVecEvents.push_back(*pEvent);
			}
		}
	}
}

void XTalentEvent::DeleteEventEffect( MUID & uid, bool bImmediatlyDelete /*= false*/ )
{
	global.emgr->DeleteXEffect(uid, bImmediatlyDelete);
}

void XTalentEvent::DeleteEventSound( MUID & uid )
{
	global.sound->StopSound(uid);
}

void XTalentEvent::TalentEvent_None( CSTalentEventInfo * pEvent )
{

}

void XTalentEvent::TalentEvent_FireEffect( CSTalentEventInfo * pEvent )
{
	if(m_pTalent)
	{
		vector<EFFECT_RESULT_DATA> vecUIDEffect;

		XEffectInvoker effectInvoker;
		effectInvoker.Invoke(m_pOwner, pEvent->m_strParam1, wstring(), pEvent->m_strParam2, &vecUIDEffect);

		for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
		{
			if((*itUID).type == CET_MODEL)
			{
				SetTalentEventEffectList((*itUID).uidEffect, pEvent);
			}
		}
	}
}

void XTalentEvent::TalentEvent_Projectile( CSTalentEventInfo * pEvent )
{

	if(m_pTalent)
	{
		// 발사체 발사
		m_pTalent->LaunchProjectile(pEvent->m_ProjectileInfo);
	}
}

void XTalentEvent::TalentEvent_Effect( CSTalentEventInfo * pEvent )
{
	// strParam1은 이펙트 이름
	if(!pEvent->m_strParam1.empty())
	{
		if(m_pOwner &&
			m_pOwner->GetActor())
		{

			EFFECT_CREATE_DATA effectCreateData;
			effectCreateData.strEffectName	= pEvent->m_strParam1;
			effectCreateData.strBoneName	= pEvent->m_strParam2;
			effectCreateData.uidActor		= m_pOwner->GetUID();
			effectCreateData.bFollow		= pEvent->m_bFollow;
			
			int nSegmentIndex = pEvent->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
			int nCapsuleIndex = pEvent->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;

			if(m_pTalent && m_pTalent->GetInfo() &&
				nSegmentIndex >= 0 && nCapsuleIndex >= 0)
			{
				// 히트 캡슐은 위치가 고정이다. 
				CalMatrixTalentHitCapsule(m_pTalent->GetInfo()->m_nID, nSegmentIndex, nCapsuleIndex, effectCreateData.matPostMatrix);
				effectCreateData.uidActor	= MUID::ZERO;
				effectCreateData.strBoneName.clear();
				effectCreateData.bFollow	= false;
			}

			MUID uidEffect = global.emgr->AddXEffect(effectCreateData);

			SetTalentEventEffectList(uidEffect, pEvent);
		}
	}
}

void XTalentEvent::TalentEvent_Sound( CSTalentEventInfo * pEvent )
{
	if ( global.sound)
		global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(pEvent->m_strParam1.c_str()).c_str(), NULL, 0, m_pOwner->GetUID(), MLocale::ConvUTF16ToAnsi(pEvent->m_strParam2.c_str()).c_str());
}

void XTalentEvent::TalentEvent_Camera( CSTalentEventInfo * pEvent )
{
	float fLen = 0.0f;

	// 내 이벤트 인가?
	if(GetOwner()->GetUID() != gvar.MyInfo.MyUID)
	{
		XObject * pObject = gg.omgr->Find(gvar.MyInfo.MyUID);
		if(pObject)
		{
			// 반경 조사
			fLen = (GetOwner()->GetPosition() - pObject->GetPosition()).Length();
		}
	}

	if(fLen <= pEvent->m_nParam1)
	{
		// 카메라 이펙트 발생
		float fDurationTime = pEvent->m_fTime2 - pEvent->m_fTime1;
		global.camera->PlayCameraEffect(pEvent->m_strParam1.c_str(), fDurationTime);
	}
}

void XTalentEvent::TalentEvent_Custom( CSTalentEventInfo * pEvent )
{
	EFFECT_POST_TRANSFORM posData;

	int nSegmentIndex = pEvent->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
	int nCapsuleIndex = pEvent->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;

	if(m_pTalent && m_pTalent->GetInfo() &&
		nSegmentIndex >= 0 && nCapsuleIndex >= 0)
	{
		CalMatrixTalentHitCapsule(m_pTalent->GetInfo()->m_nID, nSegmentIndex, nCapsuleIndex, posData.matWorldMatrix);
	}

	vector<EFFECT_RESULT_DATA> vecUIDEffect;

	if(pEvent->m_bServerDir)
		posData.matWorldMatrix = m_pTalent->GetServerStartMatrix();

	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, pEvent->m_strCustomEvent, pEvent->m_strParam1, pEvent->m_strParam2, &vecUIDEffect, NULL, &posData);

	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
	{
		if((*itUID).bLoop == true)
		{
			if((*itUID).type == CET_MODEL)
			{
				SetTalentEventEffectList((*itUID).uidEffect, pEvent);	
			}

			if((*itUID).type == CET_SOUND)
			{
				SetTalentEventSoundList(itUID->uidEffect, pEvent);
			}
		}
	}
}

void XTalentEvent::TalentEvent_Camear_Lock( CSTalentEventInfo * pEvent )
{
	// 내 이벤트 인가?
	if(GetOwner()->GetUID() != gvar.MyInfo.MyUID)
	{
		float fLen = 0.0f;
		XObject * pObject = gg.omgr->Find(gvar.MyInfo.MyUID);
		if(pObject)
		{
			// 반경 조사
			fLen = (GetOwner()->GetPosition() - pObject->GetPosition()).Length();

			if(fLen <= pEvent->m_nParam1)
			{
				// 카메라 락 발생
				XModuleMyControl* pModuleMyControl = pObject->GetModuleMyControl();
				pModuleMyControl->DoCameraLock((MUID)GetOwner()->GetUID(), pEvent->m_strParam1, pEvent->m_fTime2);

				gvar.Debug.uidCameraLock = GetOwner()->GetUID();
				gvar.Debug.strCameraLockBoneName = pEvent->m_strParam1;
			}
		}
	}
}

bool XTalentEvent::CheckEventGo()
{
	if(m_pOwner)
		return true;

	return false;
}

bool XTalentEvent::CheckLoopEffect( MUID uidEffect )
{
	XEffect* pEffect = global.emgr->GetXEffect(uidEffect);
	if(pEffect == NULL)
		return false;

	if(pEffect->IsEffectLoop())
		return true;

	return false;
}

void XTalentEvent::TalentEvent_Delayed_Act( CSTalentEventInfo * pEvent )
{
	if(m_pTalent)
	{
		// 발사체 발사
		m_pTalent->DelayAct(pEvent->m_strParam1);
	}
}

void XTalentEvent::CalMatrixTalentHitCapsule( int nTalentID, int nSegmentIndex, int nCapsuleIndex, MMatrix& matWorld )
{
	// 판정 캡슐 위치는 탤런트 시작 위치에서 계산하자.
	// 모델은 움직일 수 있어도 판정 캡슐은 이동하지 않는다.
	MCapsule hitCapsule = XTalentHelper::GetTalentHitCapule(nTalentID, nSegmentIndex, nCapsuleIndex);
	vec3 vHitCapsuleCenter = hitCapsule.GetCenter();

	RMatrix matOwenrWorld;
	matOwenrWorld.SetLocalMatrix(m_pTalent->GetStartPos(), -m_pOwner->GetDirection(), m_pOwner->GetUpvector());
	if(	m_pTalent->GetInfo()->m_bHitCapsulePosSync)
	{
		matOwenrWorld.SetLocalMatrix(m_pOwner->GetPosition(), -m_pOwner->GetDirection(), m_pOwner->GetUpvector());
	}

	matOwenrWorld.TransformVect(vHitCapsuleCenter);
	matWorld.SetLocalMatrix(vHitCapsuleCenter, -m_pOwner->GetDirection(), vec3::AXISZ);
}

void XTalentEvent::TalentEvent_GroundFireEffect( CSTalentEventInfo * pEvent )
{
	XEffectSinglyInvoker effectInvoker;
	effectInvoker.Invoke(pEvent->m_strParam1, m_pTalent->GetTargets().vTargetPos, m_pOwner->GetDirection(), vec3(vec3::AXISZ));
}
