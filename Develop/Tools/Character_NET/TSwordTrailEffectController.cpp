#include "stdafx.h"
#include "TSwordTrailEffectController.h"
#include "XCharacter.h"

//////////////////////////////////////////////////////////////////////////
// XSwordTrailEffectController
TSwordTrailEffectController::TSwordTrailEffectController()
{
	m_nMaxFrame			= 0;
	m_nFrameGapControl	= 0;
	m_nStartFrame		= 0;
	m_nEndFrame			= 0;

	m_nCurrentSwordTrailEffectIndex = -1;
}

TSwordTrailEffectController::~TSwordTrailEffectController()
{
	DestroySwordTrailEffect();
}

void TSwordTrailEffectController::DestroySwordTrailEffect()
{
	for(vector<XSwordTrailEffect*>::iterator it = m_vecSwordTrailEffect.begin(); it != m_vecSwordTrailEffect.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vecSwordTrailEffect.clear();

	m_nCurrentSwordTrailEffectIndex = -1;
	m_strCurrentAnimationName.clear();
}

void TSwordTrailEffectController::RenderSwordTrailEffect()
{
	if(m_nCurrentSwordTrailEffectIndex >= 0)
	{
		m_vecSwordTrailEffect[m_nCurrentSwordTrailEffectIndex]->RenderSwordTrailEffect(255);
	}
}

void TSwordTrailEffectController::UpdateSwordTrailEffect( float fDelta, int nCurrFrame, SWORDTRAIL_DATA* pData )
{
	int nCount = 0;
	m_nCurrentSwordTrailEffectIndex = -1;
	SWORDTRAILSAMPLING_DATA* pSamplingData;
	for(vector<SWORDTRAILSAMPLING_DATA>::iterator it = pData->vecSamplingData.begin(); it != pData->vecSamplingData.end(); it++)
	{
		if(nCurrFrame >= it->nStartFrame &&
			nCurrFrame <= it->nEndFrame)
		{
			if(nCount >= (int)m_vecSwordTrailEffect.size())
				return;

			m_nCurrentSwordTrailEffectIndex = nCount;
			pSamplingData = &(*it);
		}

		nCount++;
	}

	if(m_nCurrentSwordTrailEffectIndex >= 0 &&
		pSamplingData)
	{
		int nVertexCount = 0;
		RMatrix matWorld = g_pMainApp->GetActor()->GetWorldTransform();
		m_vecSwordTrailEffect[m_nCurrentSwordTrailEffectIndex]->CalSwordTrailEffectVertexCount(nCurrFrame, pSamplingData->nStartFrame, pSamplingData->nEndFrame, pSamplingData->nFrameGap, nVertexCount);
		m_vecSwordTrailEffect[m_nCurrentSwordTrailEffectIndex]->UpdateVertex(nVertexCount, matWorld);
	}
}

void TSwordTrailEffectController::CreateSwordTrailEffect( SWORDTRAIL_DATA* pData )
{
	m_strCurrentAnimationName = pData->strAnimationName;

	for(vector<SWORDTRAILSAMPLING_DATA>::iterator it = pData->vecSamplingData.begin(); it != pData->vecSamplingData.end(); it++)
	{
		XSwordTrailEffect* pSwordTrailEffect = new XSwordTrailEffect;
		if(pSwordTrailEffect->CreateSwordTrailEffect(g_pMainApp->GetActor(), &(*it), SWORDTRAIL_TEXTURE_NAME) == false)
		{
			SAFE_DELETE(pSwordTrailEffect);
			continue;
		}
		
		m_vecSwordTrailEffect.push_back(pSwordTrailEffect);
	}
}

bool TSwordTrailEffectController::CheckSwordTrailEffectAnimation( string& strAnimation )
{
	return (m_strCurrentAnimationName == strAnimation);
}

#include "RRenderHelper.h"
void TSwordTrailEffectController::TestRender()
{
	RRenderHelper::SetRenderState();

	//RBoundingBox box = RBoundingBox(RVector(-fSize,-fSize,-fSize),RVector(fSize,fSize,fSize));
	//RMatrix matTM = actorNode->GetWorldTransform();
	//RMatrix matScale;
	//matScale.SetScaleMatrix(fScale);
	//RRenderHelper::RenderBox(matScale * matTM,box,dwColor);

	RRenderHelper::EndRenderState();
}