#include "stdafx.h"
#include "TETransformControllerManager.h"
#include "TEBaseAnimationTC.h"
#include "RMesh.h"

TETransformControllerManager::TETransformControllerManager( TECharacter* pCharacter )
: XTransformControllerManager(pCharacter)
{
	if(pCharacter == NULL)
		return;

	CreateAnimationTC(pCharacter);
}

TETransformControllerManager::~TETransformControllerManager()
{

}

bool TETransformControllerManager::SetAnimation( tstring& strAniName, bool bForce/*=false*/, bool bSameFrame /*= false*/, bool bTestPlay /*= false*/ )
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation == NULL)
		return false;


	bool bOK = pBaseAnimation->SetAnimation(strAniName, bForce, bSameFrame);

	if(bOK)
	{
		pBaseAnimation->CheckActiveActorNode(tstring(L"dummy_grip"));
		pBaseAnimation->CheckActiveActorNode(tstring(L"dummy_throw_dir"));
	}

	return bOK;
}

void TETransformControllerManager::Update( float fDelta )
{
	// 툴도 알아서 처리...
	// 툴은 그냥 모두 업데이트...
	if( !m_pActor->GetUsable() ) return;

	m_pActor->Pause();

	vector<string> vecActiveNodeList;

	for(MAP_TC::iterator it = m_mapTransformController.begin(); it != m_mapTransformController.end(); ++it)
	{
		if ((it->second)->IsEnabled())
		{
			// 업데이트
			(it->second)->PreUpdate(fDelta);
			(it->second)->Update(fDelta);
		}
	}

	// 모든 ActorNode 업데이트
	UpdateActorNode();

	m_bUpdated = true;
	m_pActor->SetAnimationUpdated(m_bUpdated);
}

void TETransformControllerManager::CloseAnimationEvent()
{
	m_pActor->GetAnimationController().CloseEvent();
}

void TETransformControllerManager::OnLoadingComplete()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->CheckReservedAnimation();
}

float TETransformControllerManager::GetAnimationSpeed()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimationSpeed();

	return 0.0f;
}

void TETransformControllerManager::SetAnimationSpeed( float fSpeed )
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->SetAnimationSpeed(fSpeed);
}

void TETransformControllerManager::SetAnimationFrame( unsigned int nFrame )
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->SetAnimationFrame(nFrame);
}

float TETransformControllerManager::GetMaxFrameToSecond()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetMaxFrameToSecond();

	return 0.0f;
}

float TETransformControllerManager::GetCurFrameToSecond()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetCurFrameToSecond();

	return 0.0f;
}

void TETransformControllerManager::PauseAnimation()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->PauseAnimation();
}

void TETransformControllerManager::StopAnimation()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->StopAnimation();
}

void TETransformControllerManager::PlayAnimation()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->PlayAnimation();
}

tstring TETransformControllerManager::GetAnimationName()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimationName();

	return L"";
}

int TETransformControllerManager::GetAniFrame()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAniFrame();

	return 0;
}

int TETransformControllerManager::GetAniMaxFrame()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAniMaxFrame();

	return 0;
}

bool TETransformControllerManager::IsAnimationLoop()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsAnimationLoop();

	return false;
}

bool TETransformControllerManager::IsPlayDone()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsPlayDone();

	return false;
}

bool TETransformControllerManager::IsOncePlayDone()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsOncePlayDone();

	return false;
}

void TETransformControllerManager::CreateAnimationTC( TECharacter* pCharacter )
{
	// 기본 애니메이션 추가
	TEBaseAnimationTC* pTc = GetTransformController<TEBaseAnimationTC>();
	if(pTc == NULL)
	{
		pTc = new TEBaseAnimationTC(pCharacter);
		m_mapTransformController.insert( MAP_TC::value_type(TEBaseAnimationTC::GetID(), pTc));
	}

	pTc->SetEnable(true);
	pCharacter->SetBaseAnimationTC(pTc);
	pCharacter->SetAnimationConroller(pTc);
}

RAnimation* TETransformControllerManager::GetAnimation()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimation();

	return NULL;
}

bool TETransformControllerManager::ReSetAnimation()
{
	TEBaseAnimationTC* pBaseAnimation = GetTransformController<TEBaseAnimationTC>();
	if(pBaseAnimation == NULL)
		return false;

	tstring strAniName = pBaseAnimation->GetAnimationName();
	pBaseAnimation->SetAniName(tstring(L""));

	bool bOK = pBaseAnimation->SetAnimation(strAniName);

	if(bOK)
	{
		pBaseAnimation->CheckActiveActorNode(tstring(L"dummy_grip"));
		pBaseAnimation->CheckActiveActorNode(tstring(L"dummy_throw_dir"));
	}

	return bOK;
}

void TETransformControllerManager::UpdateActorNode()
{
	vector<RActorNode*>& vecNodes = m_pActor->GetActorNodes();
	for(vector<RActorNode*>::iterator it = vecNodes.begin(); it != vecNodes.end(); ++it)
	{
		(*it)->UpdateFinalResultTransforms();
	}
}