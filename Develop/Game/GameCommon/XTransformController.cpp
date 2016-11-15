#include "stdafx.h"
#include "XTransformController.h"
#include "RMesh.h"
#include "MLocale.h"

XTransformController::XTransformController( XCharacter* pCharacter )
: m_pActor(pCharacter)
, m_bEnable(false)
{
	if(m_pActor == NULL)
		return;

	m_pActor->Stop();
}

XTransformController::~XTransformController()
{
	Destroy();
}

void XTransformController::Destroy()
{

}

void XTransformController::Update( float fDelta )
{
	m_pActor->Stop();

}

ANIMATION_CHECK_STATE XTransformController::CheckAnimationLoadingComplete( tstring& strAniName )
{
	if(m_pActor->m_pMesh)
	{
		if(m_pActor->m_pMesh->IsExsitAnimationListFile() == false)
			return AS_NONE;

		if(m_pActor->m_pMesh->IsLoadedAnimaitonList() == false)
			return AS_LOADING;
	}

	RAnimation* pAniSet = m_pActor->GetAnimation( MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str() );

	if(pAniSet == NULL)
		return AS_NONE;
	
	if(pAniSet &&
		pAniSet->GetResource() &&
		pAniSet->GetResource()->GetState() == RRS_LOADED)
		return AS_LOADED;

	return AS_LOADING;
}

ANIMATION_CHECK_STATE XTransformController::CheckAnimationUsable( tstring& strAniName )
{
	if( m_pActor->GetUsable() == false) 
		return AS_MODEL_LOADING;
		
	return CheckAnimationLoadingComplete(strAniName);
}
