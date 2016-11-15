#include "StdAfx.h"
#include "XEffectWaitActorLoadingComplete.h"
#include "XEffectManager.h"
#include "XCharacter.h"

XEffectWaitActorLoadingComplete::XEffectWaitActorLoadingComplete( )
{
	m_bInit = false;
}

XEffectWaitActorLoadingComplete::~XEffectWaitActorLoadingComplete( void )
{

}

void XEffectWaitActorLoadingComplete::Init( XEffectManager * pEffectManager )
{
	m_pEffectManager = pEffectManager;
	m_bInit = true;
}

void XEffectWaitActorLoadingComplete::AddEffectWaiting( MUID uidEffect, EFFECT_CREATE_DATA effectData, WAIT_EFFECT_TYPE eType )
{
	if(m_bInit == false ||
		m_pEffectManager == NULL)
		return;

	if(uidEffect.IsInvalid() ||
		effectData.uidActor.IsInvalid())
		return;

	stEffectWaitingData effectWaitingData;

	effectWaitingData.eType								= eType;
	effectWaitingData.uidEffect							= uidEffect;
	effectWaitingData.effectData						= effectData;

	m_vecEffectWaiting.push_back(effectWaitingData);
}

void XEffectWaitActorLoadingComplete::Destroy()
{
	m_vecEffectWaiting.clear();
}

void XEffectWaitActorLoadingComplete::ActorLoadingComplete( MUID uidActor )
{
	vector<stEffectWaitingData>::iterator itEffectData = m_vecEffectWaiting.begin();
	while(itEffectData != m_vecEffectWaiting.end())
	{
		if(itEffectData->effectData.uidActor == uidActor)
		{
			XEffectBase* pEffect = NULL; 
			if(itEffectData->eType == WET_MODEL)
			{
				pEffect = m_pEffectManager->GetXEffect(itEffectData->uidEffect);
			}
			else if(itEffectData->eType == WET_WEAPON)
			{
				pEffect = m_pEffectManager->GetXWeaponEffect(itEffectData->uidEffect);
			}

			if(pEffect != NULL)
			{
				// ÀÌÆåÆ® Ãß°¡
				m_pEffectManager->AddXEffectReserve(pEffect, itEffectData->effectData);
			}

			itEffectData = m_vecEffectWaiting.erase(itEffectData);
			continue;
		}

		itEffectData++;
	}
}

bool XEffectWaitActorLoadingComplete::CheckWaiting(const MUID& uidEffect)
{
	for(vector<stEffectWaitingData>::iterator itEffectData = m_vecEffectWaiting.begin(); itEffectData != m_vecEffectWaiting.end(); ++itEffectData)
	{
		if(itEffectData->uidEffect == uidEffect)
			return true;
	}

	return false;
}

void XEffectWaitActorLoadingComplete::DelEffectWaiting( MUID uidEffect )
{
	for(vector<stEffectWaitingData>::iterator itEffectData = m_vecEffectWaiting.begin(); itEffectData != m_vecEffectWaiting.end(); ++itEffectData)
	{
		if(itEffectData->uidEffect == uidEffect)
		{
			m_vecEffectWaiting.erase(itEffectData);
			return;
		}
	}
}