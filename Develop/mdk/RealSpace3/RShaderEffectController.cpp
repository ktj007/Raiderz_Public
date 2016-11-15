#include "StdAfx.h"

#include "RShaderEffectController.h"
#include "RShaderCompositeManager.h"

namespace rs3 {

bool  RShaderEffectController::m_bRimLighting		= true;
float RShaderEffectController::m_fRimDistance		= 3000.f;
float RShaderEffectController::m_fRimFalloffStart	= 1000.f;
float RShaderEffectController::m_fRimFalloffLength	= 2000.f;
float RShaderEffectController::m_fRimFalloffRate	= 0.0005f;

RShaderEffectController::RShaderEffectController() :
	RSceneNodeController(RSNCTRL_POSTUPDATE),
	m_bAddColor(false),
	m_bGlowEffect(false),
	m_bGlowEffectFading(false),
	m_fGlowScale(1.0f)
{
	AddAttribute( RCA_STICKY );
}

RShaderEffectController::~RShaderEffectController()
{
}

bool RShaderEffectController::CheckInstancingCondition()
{
	if( IsEnableAddColorEffect() || IsEnableGlowEffect() )
		return false;

	return true;
}

void RShaderEffectController::EnableAddColorEffect( const RVector3& color )
{
	m_bAddColor = true;
	m_vAddColor = color;
}

void RShaderEffectController::DisableAddColorEffect()
{
	m_bAddColor = false;
}

void RShaderEffectController::EnableGlowEffect( const RVector3& color )
{
	m_bGlowEffect = true;
	m_bGlowEffectFading = false;
	m_vGlowColor = color;
}

void RShaderEffectController::DisableGlowEffect()
{
	m_bGlowEffectFading = true;
}

void RShaderEffectController::EnableRimLighting()
{
	m_bRimLighting = true;
}

void RShaderEffectController::DisableRimLighting()
{
	m_bRimLighting = false;
}

void RShaderEffectController::SetRimLightingDistance( float fRimDistance, float fRimFalloffStart )
{
	_ASSERT( fRimDistance >= 0.0f || fRimFalloffStart >= 0.0f );

	m_fRimDistance = fRimDistance;

	if( fRimFalloffStart > fRimDistance )
		m_fRimFalloffStart = fRimDistance;
	else
		m_fRimFalloffStart = fRimFalloffStart;

	m_fRimFalloffLength = m_fRimDistance - fRimFalloffStart;

	if( m_fRimFalloffLength > 0.0f )
		m_fRimFalloffRate = 1.0f / m_fRimFalloffLength;
	else
		m_fRimFalloffRate = 0.0f;
}


RSCID RShaderEffectController::CommitShaderMStage( RSCID* pBeforeComponent, bool bPointColor, bool bUseShade)
{
	RSCID rscComponent = *pBeforeComponent;

	if( !m_pSceneNode->GetUsable() )
		return rscComponent;

	if( m_bGlowEffect )
	{
		if( !bPointColor && bUseShade )
		{
			rscComponent |= RSC_TRANSFORM_CONSTANT | RSC_TRANSFORM | RSC_NORMAL | RSC_EXPORT_NORMAL | RSC_PPL_VIEW_DIR | RSC_PPL_COS_VIEW | RSC_PPL_GLOW | RSC_ADD_COLOR;

			RVector color = m_vGlowColor * m_fGlowScale;
			float diffuseScale = ( 1.0f - m_fGlowScale ) * 12.0f + 4.0f;

			RShaderCompositeManager::m_cGlowColorAndDiffuseRate.SetVector4( color.x, color.y, color.z, diffuseScale );
		}
	}

	if( m_bAddColor )
	{
		rscComponent |= RSC_ADD_SPECIFIC_COLOR | RSC_ADD_COLOR;

		RShaderCompositeManager::m_cAddColor.SetVector3( m_vAddColor );
	}

	return rscComponent;
}


RSCID RShaderEffectController::CommitShaderGStage(  RSCID* pBeforeComponent, bool bPointColor, bool bUseShade, float fDistanceToCamera )
{
	RSCID rscComponent = *pBeforeComponent;

	if( !m_pSceneNode->GetUsable() )
		return rscComponent;

	if( m_bRimLighting )
	{
		//RShaderCompositeManager::m_cRimScale.SetFloat( rimScale );
		rscComponent |= RSC_PPL_G_RIM;
	}

	return rscComponent;
}

void RShaderEffectController::Update(RCameraSceneNode* pCamera /*= NULL*/)
{
	RPFC_THISFUNC;
	if( !m_pSceneNode->GetUsable() )
		return;

	if( m_bGlowEffect )
	{
		float fElapsed = REngine::GetTimer().GetElapsedTime() * 0.001f;

		if( m_bGlowEffectFading )
		{
			m_fGlowScale -= fElapsed * 4.0f;
			if( m_fGlowScale < 0.0f )
			{
				m_fGlowScale = 0.0f;
				m_bGlowEffect = false;
			}
		}
		else
		{
			m_fGlowScale += fElapsed * 4.0f;
			if( m_fGlowScale > 1.0f )
			{
				m_fGlowScale = 1.0f;
			}
		}
	}
}

}