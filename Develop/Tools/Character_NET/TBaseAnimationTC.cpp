#include "stdafx.h"
#include "TBaseAnimationTC.h"

TBaseAnimationTC::TBaseAnimationTC( TCharacter* pCharacter )
:XBaseAnimationTC(pCharacter)
{

}

TBaseAnimationTC::~TBaseAnimationTC( void )
{

}

void TBaseAnimationTC::InitCurrentAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->SetFrame(0);
	m_pCurrentRAnimationController->ResetEvent();
}

void TBaseAnimationTC::ResetAnimationEventByFrame( int nFrame )
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	if(m_pCurrentRAnimationController->GetState() != RPS_PAUSE)
		return;

	for(vector<ANIMATION_EVENT>::iterator itEvent = m_pCurrentRAnimationController->GetAnimationEvent().begin();
		itEvent != m_pCurrentRAnimationController->GetAnimationEvent().end();
		itEvent++)
	{
		(*itEvent).bEventDone = true;
		if((*itEvent).pAnimationEvent->m_nFrame > nFrame)
		{
			(*itEvent).bEventDone = false;
		}
	}
}

rs3::RPLAYSTATE TBaseAnimationTC::GetState()
{
	if(m_pCurrentRAnimationController == NULL)
		return RPS_STOP;

	return m_pCurrentRAnimationController->GetState();
}

void TBaseAnimationTC::StopAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	InitCurrentAnimation();
	m_pCurrentRAnimationController->Pause();
}

void TBaseAnimationTC::ClearAnimationEvent()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->GetAnimationEvent().clear();
}

void TBaseAnimationTC::AddAnimationEvent( ANIMATION_EVENT& ani_event )
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->GetAnimationEvent().push_back(ani_event);
}

bool TBaseAnimationTC::CheckExistNodeController( string& strNodeName )
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	if(m_pCurrentRAnimationController->IsActiveNode(strNodeName))
		return true;

	return false;
}

RActorNode* TBaseAnimationTC::GetAnimatedActorNodeTransform( string& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace )
{
	_rOut.MakeIdentity();
	RActorNode* pActorNode = m_pActor->GetActorNode(strNodeName.c_str());
	if (NULL != pActorNode &&
		m_pCurrentRAnimationController != NULL)
	{
		DWORD nFrame = m_pCurrentRAnimationController->GetFrame();
		if (m_pActor->GetUpdatedFrame() != REngine::GetDevice().GetFrameCount())
		{
			bool bPlayDone = false;
			m_pCurrentRAnimationController->GetNextFrameAndState(REngine::GetTimer().GetElapsedTime(), nFrame, bPlayDone);
		}

		switch(eTransformSpace)
		{
		case E_TS_WORLD:
			{
				m_pCurrentRAnimationController->GetWorldTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		case E_TS_LOCAL:
			{
				m_pCurrentRAnimationController->GetLocalTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		case E_TS_LOCAL_RESULT:
			{
				m_pCurrentRAnimationController->GetResultTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		}
	}

	return pActorNode;
}