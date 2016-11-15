#include "StdAfx.h"
#include "TOverlayAnimationTC.h"

TOverlayAnimationTC::TOverlayAnimationTC( TCharacter* pCharacter )
:XOverlayAnimationTC(pCharacter)
, m_bTestPlay(false)
{
}

TOverlayAnimationTC::~TOverlayAnimationTC( void )
{
}

void TOverlayAnimationTC::Update( float fDelta )
{
	if(m_bTestPlay == false)
	{
		TCharacter* pCharacter = static_cast<TCharacter*>(m_pActor);
		if(pCharacter->IsPlayDone())
			OverlayAnimationStop();
	}

	XOverlayAnimationTC::Update(fDelta);
}

void TOverlayAnimationTC::SetAnimation( string& strAniName )
{
	XOverlayAnimationTC::SetAnimation(strAniName);

	OverlayAnimationPlay();
}