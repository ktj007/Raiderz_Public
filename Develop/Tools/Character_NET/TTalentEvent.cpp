#include "StdAfx.h"
#include "TTalentEvent.h"
#include "TEffectCharacterToolInvoker.h"
#include "TEffectCharacterToolSinglyInvoker.h"

//////////////////////////////////////////////////////////////////////////
TTalentEvent::TTalentEvent()
: XBaseTalentEvent()
{
	m_pActor = NULL;
	m_bPause = false;
}

TTalentEvent::~TTalentEvent()
{
	m_pActor = NULL;
}

void TTalentEvent::EventUpdate( float fDelta )
{
	if(m_bPause)
		return;

	XBaseTalentEvent::EventUpdate(fDelta);
}

void TTalentEvent::GetActTalentEvent( TALENT_EVENT eventtype, vector<CSTalentEventInfo>& outVecEvents )
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

void TTalentEvent::DeleteEventEffect( MUID & uid, bool bImmediatlyDelete /*= false*/ )
{
	g_pMainApp->m_pEffectManager->DeleteTEffect(uid, bImmediatlyDelete);
}

void TTalentEvent::DeleteEventSound( MUID & uid )
{
	g_pMainApp->m_pSoundMgr.StopDynamic3D(uid);
}

void TTalentEvent::TalentEvent_None( CSTalentEventInfo * pEvent )
{

}

void TTalentEvent::TalentEvent_FireEffect( CSTalentEventInfo * pEvent )
{
	vector<EFFECT_RESULT_DATA> vecUIDEffect;

	TEffectCharacterToolInvoker effectInvoker;
	effectInvoker.Invoke(m_pActor, pEvent->m_strParam1, string(""), pEvent->m_strParam2, &vecUIDEffect);

	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
	{
		if((*itUID).type == CET_MODEL)
		{
			SetTalentEventEffectList((*itUID).uidEffect, pEvent);
		}
	}
}

void TTalentEvent::TalentEvent_Projectile( CSTalentEventInfo * pEvent )
{
	TProjectile* pProjectile = new TProjectile();
	if(pProjectile->LaunchProjectile(pEvent->m_ProjectileInfo, m_pActor, m_pTalent) == false)
		return;

	g_pMainApp->m_vecProjectile.push_back(pProjectile);
}

void TTalentEvent::TalentEvent_Effect( CSTalentEventInfo * pEvent )
{
	// strParam1은 이펙트 이름
	if(!pEvent->m_strParam1.empty())
	{
		MUID uidEffect;

		if(m_pActor)
		{
			RMatrix matWorld;

			vec3 vPos( m_pActor->GetWorldPosition() );
			vec3 vDir( m_pActor->GetWorldDirection() );
			RActorNode * pRactorNode = m_pActor->GetActorNodePos(pEvent->m_strParam2.c_str(), vPos, E_TS_WORLD);

			matWorld.SetLocalMatrix(vPos, vDir, vec3::AXISZ);

			// 판정캡슐위치 가져오기
			int nSegmentIndex = pEvent->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
			int nCapsuleIndex = pEvent->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;

			vec3 vHitCapsuleCenter(vec3::ZERO);
			if(m_pTalent &&
				nSegmentIndex >= 0 &&
				nCapsuleIndex >= 0)
			{
				if((int)m_pTalent->m_HitInfo.m_vSegments.size() > nSegmentIndex)
				{
					if((int)m_pTalent->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules.size() > nCapsuleIndex)
					{
						MCapsule hitCapsule = m_pTalent->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules[nCapsuleIndex];

						vHitCapsuleCenter = hitCapsule.GetCenter();
						if(m_pTalent->m_bHitCapsulePosSync)
						{
							MMatrix matWorldModel;
							matWorldModel.SetLocalMatrix(m_pActor->GetPosition(), m_pActor->GetDirection(), m_pActor->GetUp());
							matWorldModel.TransformVect(vHitCapsuleCenter);
						}
					}
				}
			}

			RMatrix matLocal;
			matLocal.MakeIdentity();
			if(vHitCapsuleCenter != vec3::ZERO)
			{
				matLocal.SetLocalMatrix(vHitCapsuleCenter, m_pActor->GetDirection(), vec3::AXISZ);
			}

			if(pEvent->m_bFollow)
			{
				// 기본적으로 부모의 방향을 무시하자.
				uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(pEvent->m_strParam1, m_pActor, pRactorNode, matLocal);
			}
			else
			{
				matWorld *= matLocal;
				uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(pEvent->m_strParam1, NULL, NULL, matWorld );
			}
		}

		SetTalentEventEffectList(uidEffect, pEvent);
	}
}

void TTalentEvent::TalentEvent_Sound( CSTalentEventInfo * pEvent )
{
	g_pMainApp->m_pSoundMgr.PlayDynamic3D(pEvent->m_strParam1.c_str(), pEvent->m_strParam2.c_str(), NULL);
}

void TTalentEvent::TalentEvent_Camera( CSTalentEventInfo * pEvent )
{
	float fLen = 0.0f;

	g_pMainApp->GetCameraEffect().PlayCameraEffect(pEvent->m_strParam1.c_str(), pEvent->m_fTime2);
	//// 내 이벤트 인가?
	//if(GetOwner()->GetUID() != gvar.MyInfo.MyUID)
	//{
	//	XObject * pObject = gg.omgr->Find(gvar.MyInfo.MyUID);
	//	if(pObject)
	//	{
	//		// 반경 조사
	//		fLen = (GetOwner()->GetPosition() - pObject->GetPosition()).Length();
	//	}
	//}

	//if(fLen <= pEvent->m_nParam1)
	//{
	//	// 카메라 이펙트 발생
	//	global.camera->PlayCameraEffect(pEvent->m_strParam1.c_str(), pEvent->m_fTime2);
	//}
}

void TTalentEvent::TalentEvent_Custom( CSTalentEventInfo * pEvent )
{
	if(m_pActor == NULL)
		return ;

	// 판정캡슐위치 가져오기
	int nSegmentIndex = pEvent->m_ProjectileInfo.m_nSegmentIndexForHitCapsuleType;
	int nCapsuleIndex = pEvent->m_ProjectileInfo.m_nCapsuleIndexForHitCapsuleType;

	vector<EFFECT_RESULT_DATA> vecUIDEffect;

	RMatrix matLocal;
	matLocal.MakeIdentity();
	vec3 vHitCapsuleCenter(vec3::ZERO);
	if(m_pTalent &&
		nSegmentIndex >= 0 &&
		nCapsuleIndex >= 0)
	{
		if((int)m_pTalent->m_HitInfo.m_vSegments.size() > nSegmentIndex)
		{
			if((int)m_pTalent->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules.size() > nCapsuleIndex)
			{
				MCapsule hitCapsule = m_pTalent->m_HitInfo.m_vSegments[nSegmentIndex].m_vCapsules[nCapsuleIndex];

				vHitCapsuleCenter = hitCapsule.GetCenter();
				if(m_pTalent->m_bHitCapsulePosSync)
				{
					MMatrix matWorldModel;
					matWorldModel.SetLocalMatrix(m_pActor->GetPosition(), m_pActor->GetDirection(), m_pActor->GetUp());
					matWorldModel.TransformVect(vHitCapsuleCenter);
				}

				TEffectCharacterToolSinglyInvoker effectInvoker;
				effectInvoker.Invoke(pEvent->m_strCustomEvent, vHitCapsuleCenter, (vec3)m_pActor->GetDirection(), vec3(vec3::AXISZ), 0.0f, &vecUIDEffect);
			}
		}
	}
	else
	{
		TEffectCharacterToolInvoker effectInvoker;
		effectInvoker.Invoke(m_pActor, pEvent->m_strCustomEvent, pEvent->m_strParam1, pEvent->m_strParam2, &vecUIDEffect, &matLocal);
	}

	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
	{
		if((*itUID).type == CET_MODEL)
		{
			SetTalentEventEffectList((*itUID).uidEffect, pEvent);
		}

		if((*itUID).type == CET_SOUND)
		{
			SetTalentEventSoundList((*itUID).uidEffect, pEvent);
		}
	}
}

bool TTalentEvent::CheckEventGo()
{
	if(m_pActor)
		return true;

	return false;
}

void TTalentEvent::DelAllTalentEventEffect()
{
	vector<TIMEEVENT>::iterator it = m_vecEffectList.begin();
	while(it != m_vecEffectList.end())
	{
		DeleteEventEffect((*it).EffectDel.uidEffect, true);
		it++;
	}
}

void TTalentEvent::SetTalentEventTime( float fActTime, float fTime )
{
	m_fPreNormalEventElapsedTime = fTime;
	m_fNormalEventElapsedTime = fTime;

	float fActStartTime = fTime - fActTime;
	if(fActStartTime >= 0)
	{
		m_bActStart = true;
		m_fPreActEventElapsedTime = fTime - fActTime;
		m_fActEventElapsedTime = m_fPreActEventElapsedTime;
	}
}

void TTalentEvent::EventsStart( CSTalentEvents* pEvents )
{
	CSTalentEvents* pNewEvents = pEvents;

	if(pNewEvents->m_nRefID > 0)
	{
		// 레퍼런스 탤런트 이벤트다.
		CSTalentInfo* pTalent = TTalentHelper::GetTalentInfo(pNewEvents->m_nRefID, 0);
		if(pTalent)
		{
			pNewEvents = &(pTalent->m_Events);
		}
	}

	XBaseTalentEvent::EventsStart(pNewEvents);
}

void TTalentEvent::TalentEvent_ExtraActive_1( CSTalentEventInfo * pEvent )
{
	if(m_pTalent->m_nExtraActive == TEAT_BUFF_ENTITY)
		g_pMainApp->m_MagicArea.Setup(m_pActor, m_pTalent, pEvent);
}

void TTalentEvent::TalentEvent_ExtraActive_2( CSTalentEventInfo * pEvent )
{
	TalentEvent_ExtraActive_1(pEvent);
}

bool TTalentEvent::CheckLoopEffect( MUID uidEffect )
{
	TEffect* pEffect = g_pMainApp->m_pEffectManager->GetTEffect(uidEffect);
	if(pEffect == NULL)
		return false;

	if(pEffect->IsEffectLoop())
		return true;

	return false;
}

void TTalentEvent::TalentEvent_Delayed_Act( CSTalentEventInfo * pEvent )
{

}