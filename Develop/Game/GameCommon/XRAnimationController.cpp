#include "stdafx.h"
#include "XRAnimationController.h"
#include "RActor.h"
#include "RAnimation.h"
#include "MLocale.h"

XRAnimationController::XRAnimationController( RSNCTRL_TYPE eType /*= RSNCTRL_POSTANIMATION*/, RSNCTRL_TYPE eFrameControllerType /*= RSNCTRL_UPDATE*/ )
: RAnimationController(eType, eFrameControllerType)
{

}

XRAnimationController::~XRAnimationController()
{

}
void XRAnimationController::OnPreUpdate()
{
	if(GetFrameController().IsEnable())
		GetFrameController().Update(NULL);
}

void XRAnimationController::OnUpdate()
{
	if(IsEnable())
		Update(NULL);
}

bool XRAnimationController::PushActiveNode( tstring& strNodeName )
{
	// 애니메이션이 셋팅이 되면 초기화가 되므로 애니메이션이 변경이 될때마다 셋팅을 해주어야 합니다.
	RAnimationControllerNode* pControllerNode = GetNodeController( MLocale::ConvTCHARToAnsi(strNodeName.c_str()) );
	if(pControllerNode == NULL)
		return true;

	RActorNode* pActorNode = pControllerNode->GetNode();
	if(pActorNode &&
		pActorNode->GetBoneRefCount() > 0)
	{
		return false;
	}

	m_activeControllers.push_back(pControllerNode);

	return true;
}

bool XRAnimationController::IsActiveNode(tstring& strNodeName)
{
	for(vector<RAnimationControllerNode*>::iterator it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
	{
		if(MLocale::ConvAnsiToTCHAR((*it)->GetNodeName()) == strNodeName)
			return true;
	}

	return false;
}

void XRAnimationController::GetLocalTransformAt( DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode )
{
	if(pActorNode == NULL)
		return;

	RAnimationControllerNode* pNode = GetNodeController( pActorNode->GetNodeName() );
	if(pNode)
		pNode->GetLocalTransformAt( nFrame, outMatrix );
	else
		outMatrix = pActorNode->GetLocalTransform();
}

void XRAnimationController::GetWorldTransformAt( DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode )
{
	if(pActorNode == NULL)
		return;

	GetResultTransformAt(nFrame, outMatrix, pActorNode);
	outMatrix *= m_pActor->GetWorldTransform();
}

void XRAnimationController::GetResultTransformAt( DWORD nFrame, RMatrix& outMatrix, RActorNode* pActorNode )
{
	if(pActorNode == NULL)
		return;

	RActorNode* pParent = pActorNode->GetParentActorNode();

	RMatrix matLocal;
	GetLocalTransformAt( nFrame, matLocal, pActorNode);

	if(pParent)
	{
		RMatrix matParent;
		GetResultTransformAt( nFrame, matParent, pParent );

		matLocal.MultiplyTo( matParent, outMatrix);
	}
	else
	{
		outMatrix = matLocal;
	}
}

void XRAnimationController::DisableAniamtionEventPreFrame( unsigned int nFrame )
{
	for(vector<ANIMATION_EVENT>::iterator itEvent = GetAnimationEvent().begin();
		itEvent != GetAnimationEvent().end();
		itEvent++)
	{
		(*itEvent).bEventDone = true;
		if((*itEvent).pAnimationEvent->m_nFrame > nFrame ||
			itEvent->pAnimationEvent->m_bUnconditional)
		{
			(*itEvent).bEventDone = false;
		}
	}
}

bool XRAnimationController::IsAnimationLoop()
{
	RAnimation* pAni = GetAnimation();
	return (pAni && pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false;
}

void XRAnimationController::ResetAniamtionSpeed()
{
	SetSpeed(GetOrgAniamtionSpeed());
}

float XRAnimationController::GetOrgAniamtionSpeed()
{
	RAnimation* pAnimation = GetAnimation();
	if(pAnimation == NULL)
		return 1.0f;

	return pAnimation->GetSpeed();
}

RAnimationControllerNode* XRAnimationController::GetControllerNode( tstring& strNodeName )
{
	for(vector<RAnimationControllerNode*>::iterator it = m_activeControllers.begin(); it != m_activeControllers.end(); ++it)
	{
		if(MLocale::ConvAnsiToTCHAR((*it)->GetNodeName()) == strNodeName)
			return *it;
	}

	return NULL;
}
