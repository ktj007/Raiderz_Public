#include "stdafx.h"
#include "TTransformControllerManager.h"
#include "TCharacter.h"

#include "TFaceAnimationTC.h"
#include "TBaseAnimationTC.h"
#include "XUpperAnimationTC.h"
#include "TOverlayAnimationTC.h"
#include "XFakeBeatenAnimationTC.h"

#include "RMesh.h"

TTransformControllerManager::TTransformControllerManager( TCharacter* pCharacter )
: XTransformControllerManager(pCharacter)
{
	if(pCharacter == NULL)
		return;

	CreateAnimationTC(pCharacter);
}

TTransformControllerManager::~TTransformControllerManager()
{

}

bool TTransformControllerManager::SetAnimation( string& strAniName, bool bForce/*=false*/, bool bSameFrame /*= false*/, bool bTestPlay /*= false*/ )
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation == NULL)
		return false;


	bool bOK = pBaseAnimation->SetAnimation(strAniName, bForce, bSameFrame);

	if(bOK)
	{
		pBaseAnimation->CheckActiveActorNode(string("dummy_grip"));
		pBaseAnimation->CheckActiveActorNode(string("dummy_throw_dir"));

		TOverlayAnimationTC* pOverlayAnimation = GetTransformController<TOverlayAnimationTC>();
		if(pOverlayAnimation)
		{
			pOverlayAnimation->SetAnimation(strAniName);
			pOverlayAnimation->SetPlayTest(bTestPlay);
		}
	}

	StopFaceAnimation();

	// UpperAnimation이 Setting 되어 있을경우 다른 Animation에 의도하지 않게 적용되므로
	// InitUpperAnimation()를 불러 적용되지 않게 해줌			_by tripleJ 110519
	InitUpperAnimation();

	return bOK;
}

void TTransformControllerManager::Update( float fDelta )
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
			(it->second)->PostUpdate(fDelta);
		}
	}

	// 모든 ActorNode 업데이트
	UpdateActorNode();

	m_bUpdated = true;
	m_pActor->SetAnimationUpdated(m_bUpdated);
}

void TTransformControllerManager::CloseAnimationEvent()
{
	m_pActor->GetAnimationController().CloseEvent();
}

bool TTransformControllerManager::SetUpperAnimation( string strUpperAniName )
{
	if( m_pActor->GetUsable() == false )
	{
		return false;
	}

	// 기본 애니메이션 추가
	XUpperAnimationTC* pTc = Add<XUpperAnimationTC>();
	if(pTc == NULL)
	{
		return false;
	}

	pTc->SetEnable(true);

	return pTc->SetAnimation(strUpperAniName);
}

bool TTransformControllerManager::IsUpperAnimationOncePlayDone()
{
	XUpperAnimationTC* pUpperAnimation = GetTransformController<XUpperAnimationTC>();
	if(pUpperAnimation)
		return pUpperAnimation->IsOncePlayDone();

	return false;
}

bool TTransformControllerManager::IsUpperAnimationPlayDone()
{
	XUpperAnimationTC* pUpperAnimation = GetTransformController<XUpperAnimationTC>();
	if(pUpperAnimation)
		return pUpperAnimation->IsPlayDone();

	return false;
}

bool TTransformControllerManager::IsUpperAnimationLoop()
{
	XUpperAnimationTC* pUpperAnimation = GetTransformController<XUpperAnimationTC>();
	if(pUpperAnimation)
		return pUpperAnimation->IsLoop();

	return false;
}

void TTransformControllerManager::InitUpperAnimation()
{
	XUpperAnimationTC* pUpperAnimation = GetTransformController<XUpperAnimationTC>();
	if(pUpperAnimation)
		return pUpperAnimation->Init();
}

void TTransformControllerManager::SetFaceAnimationActive( bool bActive )
{
	TFaceAnimationTC* pFaceAnimation = GetTransformController<TFaceAnimationTC>();
	if(pFaceAnimation)
		pFaceAnimation->SetEnable(bActive);
}

void TTransformControllerManager::SetFaceAnimationNPC( bool bNPC, TCharacter * pActor )
{
	// 페이스 애니메이션 추가
	TFaceAnimationTC* pTC = GetTransformController<TFaceAnimationTC>();
	if(pTC == NULL)
	{
		pTC = new TFaceAnimationTC(pActor);
		m_mapTransformController.insert( MAP_TC::value_type(TFaceAnimationTC::GetID(), pTC));
	}

	pTC->SetEnable(true);
	pTC->SetNPC(bNPC);
}

void TTransformControllerManager::OnLoadingComplete()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->CheckReservedAnimation();

	TOverlayAnimationTC* pOverlayAnimation = GetTransformController<TOverlayAnimationTC>();
	if(pOverlayAnimation)
		pOverlayAnimation->CheckReservedAnimation();
}

float TTransformControllerManager::GetAnimationSpeed()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimationSpeed();

	return 0.0f;
}

void TTransformControllerManager::SetAnimationSpeed( float fSpeed )
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->SetAnimationSpeed(fSpeed);
}

void TTransformControllerManager::SetAnimationFrame( unsigned int nFrame )
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->SetAnimationFrame(nFrame);
}

float TTransformControllerManager::GetMaxFrameToSecond()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetMaxFrameToSecond();

	return 0.0f;
}

float TTransformControllerManager::GetCurFrameToSecond()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetCurFrameToSecond();

	return 0.0f;
}

void TTransformControllerManager::PauseAnimation()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->PauseAnimation();

	TOverlayAnimationTC* pOverlayAnimation = GetTransformController<TOverlayAnimationTC>();
	if(pOverlayAnimation)
		pOverlayAnimation->OverlayAnimationPause();
}

void TTransformControllerManager::StopAnimation()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->StopAnimation();

	TOverlayAnimationTC* pOverlayAnimation = GetTransformController<TOverlayAnimationTC>();
	if(pOverlayAnimation)
		pOverlayAnimation->OverlayAnimationStop();

	// _by tripleJ 110519
	/*
	XUpperAnimationTC* pUpperAnimation		= GetTransformController<XUpperAnimationTC>();
	if( pUpperAnimation )
		pUpperAnimation->Stop();
		*/
}

void TTransformControllerManager::PlayAnimation()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		pBaseAnimation->PlayAnimation();

	TOverlayAnimationTC* pOverlayAnimation = GetTransformController<TOverlayAnimationTC>();
	if(pOverlayAnimation)
		pOverlayAnimation->OverlayAnimationPlay();
}

string TTransformControllerManager::GetAnimationName()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimationName();

	return "";
}

int TTransformControllerManager::GetAniFrame()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAniFrame();

	return 0;
}

int TTransformControllerManager::GetAniMaxFrame()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAniMaxFrame();

	return 0;
}

bool TTransformControllerManager::IsAnimationLoop()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsAnimationLoop();

	return false;
}

bool TTransformControllerManager::IsPlayDone()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsPlayDone();

	return false;
}

bool TTransformControllerManager::IsOncePlayDone()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->IsOncePlayDone();

	return false;
}

void TTransformControllerManager::CreateAnimationTC( TCharacter* pCharacter )
{
	// 기본 애니메이션 추가
	TBaseAnimationTC* pTc = GetTransformController<TBaseAnimationTC>();
	if(pTc == NULL)
	{
		pTc = new TBaseAnimationTC(pCharacter);
		m_mapTransformController.insert( MAP_TC::value_type(TBaseAnimationTC::GetID(), pTc));
	}

	pTc->SetEnable(true);
	pCharacter->SetBaseAnimationTC(pTc);
	pCharacter->SetAnimationConroller(pTc);

	// 오버레이 내이메이션 추가
	TOverlayAnimationTC* pOverlayTc = GetTransformController<TOverlayAnimationTC>();
	if(pOverlayTc == NULL)
	{
		pOverlayTc = new TOverlayAnimationTC(pCharacter);
		m_mapTransformController.insert( MAP_TC::value_type(TOverlayAnimationTC::GetID(), pOverlayTc));
	}

	pOverlayTc->SetEnable(true);
}

RAnimation* TTransformControllerManager::GetAnimation()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation)
		return pBaseAnimation->GetAnimation();

	return NULL;
}

bool TTransformControllerManager::ReSetAnimation()
{
	TBaseAnimationTC* pBaseAnimation = GetTransformController<TBaseAnimationTC>();
	if(pBaseAnimation == NULL)
		return false;

	string strAniName = pBaseAnimation->GetAnimationName();
	pBaseAnimation->SetAniName(string(""));

	bool bOK = pBaseAnimation->SetAnimation(strAniName);

	if(bOK)
	{
		pBaseAnimation->CheckActiveActorNode(string("dummy_grip"));
		pBaseAnimation->CheckActiveActorNode(string("dummy_throw_dir"));
	}

	return bOK;
}

void TTransformControllerManager::UpdateActorNode()
{
	vector<RActorNode*>& vecNodes = m_pActor->GetActorNodes();
	for(vector<RActorNode*>::iterator it = vecNodes.begin(); it != vecNodes.end(); ++it)
	{
		(*it)->UpdateAllParentDependentValues();
		(*it)->UpdateFinalResultTransforms();
	}
}

void TTransformControllerManager::StartFaceAnimation( string& strAniName, bool bEye )
{
	TFaceAnimationTC* pFaceAnimation = GetTransformController<TFaceAnimationTC>();
	if(pFaceAnimation)
		pFaceAnimation->StartFaceAnimation(strAniName, bEye);
}

void TTransformControllerManager::EndFaceAnimation( string& strAniName, float fEndBledTime )
{
	TFaceAnimationTC* pFaceAnimation = GetTransformController<TFaceAnimationTC>();
	if(pFaceAnimation)
		pFaceAnimation->EndFaceAnimation(strAniName, fEndBledTime);

}

void TTransformControllerManager::StopFaceAnimation()
{
	TFaceAnimationTC* pFaceAnimation = GetTransformController<TFaceAnimationTC>();
	if(pFaceAnimation)
		pFaceAnimation->StopFaceAnimation();
}

void TTransformControllerManager::InitFakeBeatenAnimation( tstring& strAniName, float fWeight, float fSpeed )
{
	XFakeBeatenAnimationTC * pTC = Add<XFakeBeatenAnimationTC>();
	pTC->Init(strAniName, fWeight, fSpeed);
}

void TTransformControllerManager::StartFakeBeaten( const TCHAR* szCurAni, const float fWeightRate, const float fSpeedRate, const TCHAR* szAniPrefix/*=NULL*/ )
{
	XFakeBeatenAnimationTC* pTC = GetTransformController<XFakeBeatenAnimationTC>();
	if (pTC)
	{
		pTC->StartBeaten(szCurAni, fWeightRate, fSpeedRate, szAniPrefix);
	}
}
