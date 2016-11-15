#include "stdafx.h"
#include "XTalentInfo.h"
#include "MUtil.h"
#include "MMath.h"
#include "MLocale.h"
#include "CSChrInfo.h"
#include "MCsvParser.h"
#include "XTalentEffectInfo.h"
#include "XStrings.h"

void XTalentInfo::CopyAll( CSTalentInfo* pTalentInfo )
{
	XTalentInfo* pxTalentInfo = static_cast<XTalentInfo*>(pTalentInfo);
	*pxTalentInfo = *this;

	// Clone에서 복사하지 않는 항목 ---

	pxTalentInfo->m_Modes.ClearButNotDelete();
	pxTalentInfo->m_EffectDataMgr.Clear();		// talent_effect_info.xml
}

CSProjectileInfo * XTalentInfo::GetProjectileInfo()
{
	if(m_Events.HasEvent(TE_LAUNCH_PROJECTILE))
	{
		for(vector<CSTalentEventInfo>::iterator itActEvent = m_Events.m_vecActEvents.begin(); itActEvent != m_Events.m_vecActEvents.end(); itActEvent++)
		{
			if((*itActEvent).m_nEvent == TE_LAUNCH_PROJECTILE)
			{
				return &((*itActEvent).m_ProjectileInfo);
			}
		}
	}

	return NULL;
}

vec3 XTalentInfo::GetSwordHitDir( float fAniTime, tstring strMeshName )
{
	XTalentEffectInfo * pTalentEffect = m_EffectDataMgr.Get(HIT_EFFECT_POSDIR);
	if(pTalentEffect)
	{
		for(vector<XTalentHitEffectPosDir>::iterator it = pTalentEffect->m_vecTalentHitEffectPosDir.begin(); it != pTalentEffect->m_vecTalentHitEffectPosDir.end(); ++it)
		{
			if(it->fHitTime == fAniTime &&
				it->strMeshName == strMeshName)
				return it->vSwordTrailHitUp;
		}
	}

	return vec3::AXISZ;
}

vec3 XTalentInfo::GetSwordHitPos( float fAniTime, tstring strMeshName )
{
	XTalentEffectInfo * pTalentEffect = m_EffectDataMgr.Get(HIT_EFFECT_POSDIR);
	if(pTalentEffect)
	{
		for(vector<XTalentHitEffectPosDir>::iterator it = pTalentEffect->m_vecTalentHitEffectPosDir.begin(); it != pTalentEffect->m_vecTalentHitEffectPosDir.end(); ++it)
		{
			if(it->fHitTime == fAniTime &&
				it->strMeshName == strMeshName)
				return it->vHitEffectPos;
		}
	}

	return vec3::ZERO;
}

XTalentEffectInfo * XTalentEffectDataMgr::Get(TALENT_EFFECT_TYPE nType)
{
	vector<XTalentEffectInfo *>::iterator itInfo = begin();
	while(itInfo != end())
	{
		if((*itInfo)->m_nEffectType == nType)
		{
			return *itInfo;
		}

		itInfo++;
	}	

	return NULL;
}

bool XTalentInfo::IsPortalTalent()
{
	return (TEAT_PORTAL == m_nExtraActive || TEAT_PORTAL_SOULBINDING == m_nExtraActive);
}

const TCHAR* XTalentInfo::GetName(void)
{
	return XStringResMgr::GetInstance()->GetString(m_szName);
}

const TCHAR* XTalentInfo::GetDesc(void)
{
	return XStringResMgr::GetInstance()->GetString(m_szDesc);
}
