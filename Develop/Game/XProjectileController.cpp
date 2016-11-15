#include "stdafx.h"
#include "XProjectileController.h"
#include "XProjectile.h"
#include "XModuleCollision.h"
#include "XModuleEffect.h"

XProjectileController::XProjectileController()
{

}

XProjectileController::~XProjectileController()
{

}

void XProjectileController::ProjectileHit( XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo )
{
	ProjectileColJudgment(pTarget, pAttacker, pTalentInfo, PCJT_HIT);
}

void XProjectileController::ProjectileDefence( XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo )
{
	ProjectileColJudgment(pTarget, pAttacker, pTalentInfo, PCJT_DEFENCE);
}

void XProjectileController::ProjectileColJudgment( XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo, ProjectileColJudgmentType colType )
{
	if(pTarget == NULL || pAttacker == NULL || pTalentInfo == NULL)
	{
		return;
	}

	m_pOwner = pTarget;

	// 나와 관련된 발사체를 찾는다.
	vector<XProjectile*> vecProjectile;
	GetMyProjectile(pAttacker, pTalentInfo, vecProjectile);

	if(vecProjectile.empty())
	{
		// 없어? 근데... 판정이네...
		// 그래두 판정 결과를 보여주자.
		ShowEmptyProjrctileEffect(pTarget, pAttacker, pTalentInfo, colType);
		return;
	}

	// 충돌된 발사체를 찾는다.
	XProjectile* pProjectile = GetEndProjectile(pTarget, pTalentInfo, vecProjectile);

	// 충돌판정결과를 알린다.
	if(pProjectile)
		pProjectile->ProjectileColResult(colType, m_pOwner);
	else
		ShowEmptyProjrctileEffect(pTarget, pAttacker, pTalentInfo, colType);
}

void XProjectileController::GetMyProjectile( XObject* pAttacker, XTalentInfo* pTalentInfo, vector<XProjectile*>& outvecMyProjectile )
{
	XObjectMap* pObjectMap = gg.omgr->GetObjectMap(XOBJ_PROJECTILE);
	if(pObjectMap == NULL)
		return;

	for(XObjectMap::iterator itObject = pObjectMap->begin(); itObject != pObjectMap->end(); itObject++)
	{
		XProjectile* pProjectile = static_cast<XProjectile*>((*itObject).second);
		if(pProjectile->IsEqualProjectile(pAttacker->GetUID(), pTalentInfo->m_nID))
		{
			outvecMyProjectile.push_back(pProjectile);
		}
	}
}

XProjectile* XProjectileController::GetEndProjectile( XObject* pTarget, XTalentInfo* pTalentInfo, vector<XProjectile*>& vecMyProjectile )
{
	for(vector<XProjectile*>::iterator itProjectile = vecMyProjectile.begin(); itProjectile != vecMyProjectile.end(); itProjectile++)
	{
		XProjectile* pProjectile = (*itProjectile);
		if(pProjectile->IsWaitColJudgment())
		{
			// 끝났어요.
			// 충돌 타겟이 판정 타겟과 같냐?
			if(pProjectile->GetColObject() == pTarget)
			{
				return pProjectile;
			}
		}
	}

	// 충돌이 안되었다면... 가까운것을 찾는다.
	return GetNearProjectile(vecMyProjectile, pTalentInfo);
}

XProjectile* XProjectileController::GetNearProjectile( vector<XProjectile*>& vecMyProjectile, XTalentInfo* pTalentInfo )
{
	XProjectile* pNearProjectile = NULL;
	float fMinLen = FLT_MAX;

	for(vector<XProjectile*>::iterator itProjectile = vecMyProjectile.begin(); itProjectile != vecMyProjectile.end(); itProjectile++)
	{
		XProjectile* pProjectile = (*itProjectile);
		float fLen = (m_pOwner->GetPosition() - pProjectile->GetPosition()).Length();
		if(fMinLen > fLen &&
			pTalentInfo->m_nID == pProjectile->GetTalentInfo()->m_nID)			// 발사체를 사용한 탤런트가 같아야 한다.
		{
			fMinLen = fLen;
			pNearProjectile = pProjectile;
		}
	}

	return pNearProjectile;
}

void XProjectileController::ShowEmptyProjrctileEffect( XObject* pTarget, XObject* pAttacker, XTalentInfo* pTalentInfo, ProjectileColJudgmentType colType )
{
	// 발사체가 없어두 이펙트를 보여주자.
	CSProjectileInfo* pProjectileInfo = GetProjectileInfo(pTalentInfo);
	if(pProjectileInfo == NULL)
		return;

	// 발사체가 히트캡슐이라면 별도 이펙트가 필요없을까? 일단 되돌린다.
	if(pProjectileInfo->m_nType == TPT_HITCAPSULE_GUIDED)
		return;

	// 오브젝트 충돌을 검사해서 캡슐에 맞는 위치를 가져온다.
	XModuleCollision *pModuleCollision = pTarget->GetModuleCollision();
	if(pModuleCollision == NULL)
		return;

	XModuleEffect* pModuleEffect = pTarget->GetModuleEffect();
	if(pModuleEffect == NULL)
		return;

	XHitDamageEffectEventData * pEventData = new XHitDamageEffectEventData;
	if(pAttacker)	pEventData->m_uidAttacker	= pAttacker->GetUID();
	if(pTarget)		pEventData->m_uidTarget		= pTarget->GetUID();
	pEventData->m_pProjectileInfo = pProjectileInfo;
	pEventData->m_eColType		= colType;
	pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_SHOW_PROJECTIL_DAMAGE, pEventData);
}

CSProjectileInfo* XProjectileController::GetProjectileInfo( XTalentInfo* pTalentInfo )
{
	for(vector<CSTalentEventInfo>::iterator itNor = pTalentInfo->m_Events.m_vecEvents.begin(); itNor != pTalentInfo->m_Events.m_vecEvents.end(); itNor++)
	{
		if((*itNor).m_nEvent == TE_LAUNCH_PROJECTILE)
		{
			return &((*itNor).m_ProjectileInfo);
		}
	}

	for(vector<CSTalentEventInfo>::iterator itAct = pTalentInfo->m_Events.m_vecActEvents.begin(); itAct != pTalentInfo->m_Events.m_vecActEvents.end(); itAct++)
	{
		if((*itAct).m_nEvent == TE_LAUNCH_PROJECTILE)
		{
			return &((*itAct).m_ProjectileInfo);
		}
	}

	return NULL;
}