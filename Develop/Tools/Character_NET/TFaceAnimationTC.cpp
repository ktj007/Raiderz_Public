#include "StdAfx.h"
#include "TFaceAnimationTC.h"

TFaceAnimationTC::TFaceAnimationTC( TCharacter* pCharacter )
: XFaceAnimationTC(pCharacter)
{

}

TFaceAnimationTC::~TFaceAnimationTC( void )
{

}

void TFaceAnimationTC::StopFaceAnimation()
{
	m_pBlinkAnimationController->Play();

	if(m_vecFaceAnimationController.empty())
		return;

	for(FaceAnimationList::iterator it = m_vecFaceAnimationController.begin(); it != m_vecFaceAnimationController.end(); ++it)
	{
		it->pFaceAnimationController->Reset();
		m_pActor->RemoveController(it->pFaceAnimationController);
		SAFE_DELETE(it->pFaceAnimationController);
	}

	m_vecFaceAnimationController.clear();

	for(FaceAnimationList::iterator it = m_vecEyeAnimationController.begin(); it != m_vecEyeAnimationController.end(); ++it)
	{
		it->pFaceAnimationController->Reset();
		m_pActor->RemoveController(it->pFaceAnimationController);
		SAFE_DELETE(it->pFaceAnimationController);
	}

	m_vecEyeAnimationController.clear();
}