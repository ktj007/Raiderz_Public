#include "stdafx.h"
#include "TEBaseAnimationTC.h"

TEBaseAnimationTC::TEBaseAnimationTC( TECharacter* pCharacter )
:XBaseAnimationTC(pCharacter)
{

}

TEBaseAnimationTC::~TEBaseAnimationTC( void )
{

}

void TEBaseAnimationTC::InitCurrentAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->SetFrame(0);
	m_pCurrentRAnimationController->ResetEvent();
}

void TEBaseAnimationTC::ResetAnimationEventByFrame( int nFrame )
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

rs3::RPLAYSTATE TEBaseAnimationTC::GetState()
{
	if(m_pCurrentRAnimationController == NULL)
		return RPS_STOP;

	return m_pCurrentRAnimationController->GetState();
}

void TEBaseAnimationTC::StopAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	InitCurrentAnimation();
	m_pCurrentRAnimationController->Pause();
}

void TEBaseAnimationTC::ClearAnimationEvent()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->GetAnimationEvent().clear();
}

void TEBaseAnimationTC::AddAnimationEvent( ANIMATION_EVENT& ani_event )
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->GetAnimationEvent().push_back(ani_event);
}

bool TEBaseAnimationTC::CheckExistNodeController( string& strNodeName )
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	if(m_pCurrentRAnimationController->GetNodeController(strNodeName))
		return true;

	return false;
}

RActorNode* TEBaseAnimationTC::GetAnimatedActorNodeTransform( tstring& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace )
{
	USES_CONVERSION_EX;
	_rOut.MakeIdentity();
	RActorNode* pActorNode = m_pActor->GetActorNode(W2A_EX(strNodeName.c_str(), 0));
	if (NULL != pActorNode &&
		m_pCurrentRAnimationController != NULL)
	{
		DWORD nFrame = m_pCurrentRAnimationController->GetFrame();

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