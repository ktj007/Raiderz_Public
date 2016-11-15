#include "stdafx.h"
#include "XTalentEffectPreLoader.h"
#include "XEffectInvoker.h"
#include "XTalentInfo.h"
#include "XTalentInfoMgr.h"

XTalentEffectPreLoader::XTalentEffectPreLoader()
{

}

XTalentEffectPreLoader::~XTalentEffectPreLoader()
{

}

void XTalentEffectPreLoader::PreLoadTalentEffect( int nTalentID )
{
	//////////////////////////////////////////////////////////////////////////
	// 준비
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if(pTalentInfo == NULL)
		return;

	PreLoaderTalentEffectInfo(pTalentInfo);
	PreLoaderTalentEventEffect(pTalentInfo);
}

void XTalentEffectPreLoader::PreLoaderTalentEffectInfo( XTalentInfo* pTalentInfo )
{
	XTalentEffectDataMgr* pTalentEffectDataMgr = pTalentInfo->GetEffectDataMgr();

	//////////////////////////////////////////////////////////////////////////
	// 과련된 모든 이펙트 로딩
	vec3 vDir = vec3::ZERO;
	vector<XTalentEffectInfo*>::iterator it = pTalentEffectDataMgr->begin();
	while(it != pTalentEffectDataMgr->end())
	{
		XEffectAdvenceInvoker advenceInvoker;
		advenceInvoker.Invoke((*it)->m_strEffectName);

		it++;
	}
}

void XTalentEffectPreLoader::PreLoaderTalentEventEffect( XTalentInfo* pTalentInfo )
{
	for(vector<CSTalentEventInfo>::iterator itNor = pTalentInfo->m_Events.m_vecEvents.begin(); itNor != pTalentInfo->m_Events.m_vecEvents.end(); itNor++)
	{
		PreLoaderTalentEventEffectInvoke(*itNor);
	}
	
	for(vector<CSTalentEventInfo>::iterator itAct = pTalentInfo->m_Events.m_vecActEvents.begin(); itAct != pTalentInfo->m_Events.m_vecActEvents.end(); itAct++)
	{
		PreLoaderTalentEventEffectInvoke(*itAct);
	}
}

void XTalentEffectPreLoader::PreLoaderTalentEventEffectInvoke( CSTalentEventInfo& eventInfo )
{
	XEffectAdvenceInvoker advenceInvoker;

	if(eventInfo.m_nEvent == TE_EFFECT || eventInfo.m_nEvent == TE_FIRE_EFFECT)
	{
		advenceInvoker.Invoke(eventInfo.m_strParam1);
	}
	else if(eventInfo.m_nEvent == TE_LAUNCH_PROJECTILE)
	{
		advenceInvoker.Invoke(eventInfo.m_ProjectileInfo.m_strEffectName);
		advenceInvoker.Invoke(eventInfo.m_ProjectileInfo.m_strAttackEffectName);
		advenceInvoker.Invoke(eventInfo.m_ProjectileInfo.m_strHitGroundEffectName);
		advenceInvoker.Invoke(eventInfo.m_ProjectileInfo.m_strFireEffectName);
	}
}