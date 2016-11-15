#include "stdafx.h"
#include ".\animationeventhandler.h"
#include "REffectMgr.h"
#include "RActor.h"
#include "REffectInstance.h"
#include "RConfiguration.h"

// listener , 테스트 리스너이며 저장되지 않는다
AnimationEventListener::~AnimationEventListener()
{
	Clear();
}

void AnimationEventListener::Clear()
{
	while(!empty())
	{
		delete begin()->second;
		erase(begin());
	}
}

void AnimationEventListener::DeleteHandler(AnimationEventHandler* pHandler)
{
	for(iterator itr = begin(); itr!=end(); )
	{
		if(itr->second==pHandler)
		{
			delete itr->second;
			itr = erase(itr);
		}
		else
			itr++;
	}
}

void AnimationEventListener::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	iterator itr = find(pEvent->m_strEvent);
	while(itr!=end() && (itr->first == pEvent->m_strEvent))
	{
		itr->second->Do();
		itr++;
	}
}

/////////////// SoulHuntAnimationEventListener
map<string,ANIMATIONEVENTHANDLER> SoulHuntAnimationEventListener::m_eventHandlers;

SoulHuntAnimationEventListener::SoulHuntAnimationEventListener()
{
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_walk",	OnAnimationEvent_walk) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_effect", OnAnimationEvent_effect) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_sound",	OnAnimationEvent_sound) );
}

SoulHuntAnimationEventListener::~SoulHuntAnimationEventListener()
{

}

void SoulHuntAnimationEventListener::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	ANIMATIONEVENTMAP::iterator itr = m_eventHandlers.find(pEvent->m_strEvent);
	if(itr!=m_eventHandlers.end())
	{
		itr->second(pEvent);
	}
	else
 		::AnimationEventListener::OnAnimationEvent(pActor,pEvent);
}

// [9/19/2006 madduck]
// TODO : 바닥에 따라 다른 소리.
void SoulHuntAnimationEventListener::OnAnimationEvent_walk(const RAnimationEvent *pEvent)
{
	string str = config.GetSoundPath();
	str += "walk.wav";

	PlaySound(str.c_str(), NULL,SND_ASYNC);
	
// 	RSoundSource* pSSource = global.sound->GetSoundSource("walk",0);
// 	MVector3 vc = entity.m_vPos;
// 	global.sound->SetListenerPos(&vc);
// 	pSSource->SetPos(&vc);
// 	//pSSource->m_vPos = entity.m_vPos;
// 	global.sound->PlaySound3D(pSSource);
}

void SoulHuntAnimationEventListener::OnAnimationEvent_effect(const RAnimationEvent *pEvent)
{
	if( pEvent->m_strParam1.empty()) return;

	REffectInstance* pEffect = REffectMgr::GetInstance()->CreateEffect(pEvent->m_strParam1.c_str());

	ASSERT(pEffect!=NULL);

	pEffect->m_bSticky = true;
	pEffect->m_bCreatedByManager = true;
	pEffect->Start();

	RActor* pActor = GetMainView()->GetActor();
	if(!pEvent->m_strParam2.empty())
	{
		RActorNode *pParentActorNode = pActor->GetActorNode(pEvent->m_strParam2.c_str());
		pEffect->SetParent(pParentActorNode);
	}

	pActor->AddChild(pEffect);
}

void SoulHuntAnimationEventListener::OnAnimationEvent_sound(const RAnimationEvent *pEvent)
{
	string str = config.GetSoundPath();
	str += pEvent->m_strParam1;
	PlaySound(str.c_str(),NULL,SND_ASYNC);
}

///////////// handler

AnimationEventHandler::AnimationEventHandler(void)
{
}

AnimationEventHandler::~AnimationEventHandler(void)
{
}

bool AnimationEventHandler::Do()
{
	if(!m_strEffectFileName.empty())
	{
		REffectInstance* pEffect = REffectMgr::GetInstance()->CreateEffect(m_strEffectFileName.c_str());

		ASSERT(pEffect!=NULL);

		pEffect->m_bSticky = true;
		pEffect->m_bCreatedByManager = true;
		pEffect->Start();
		
		RActor* pActor = GetMainView()->GetActor();
		if(!m_strEffectParent.empty())
		{
			RActorNode *pParentActorNode = pActor->GetActorNode(m_strEffectParent.c_str());
			pEffect->SetParent(pParentActorNode);
		}

		pActor->AddChild(pEffect);
	}

	if(!m_strSoundFileName.empty())
	{
		PlaySound(m_strSoundFileName.c_str(),NULL,SND_ASYNC);
	}

	return true;
}
