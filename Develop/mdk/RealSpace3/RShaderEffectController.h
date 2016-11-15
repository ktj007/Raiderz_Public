#pragma once

#include "RSceneNodeController.h"
#include "RShaderComponentID.h"

namespace rs3 {

class RS_API RShaderEffectController : public RSceneNodeController
{
public:
	DECLARE_NODE_ID_STRING("ShaderEffectController");

	RShaderEffectController();
	virtual ~RShaderEffectController();

	bool CheckInstancingCondition();

	void EnableAddColorEffect( const RVector3& color );
	void DisableAddColorEffect();
	bool IsEnableAddColorEffect()	{ return m_bAddColor;}

	void EnableGlowEffect( const RVector3& color );
	void DisableGlowEffect();
	bool IsEnableGlowEffect()	{ return m_bGlowEffect;}

	static void EnableRimLighting();
	static void DisableRimLighting();
	static void SetRimLightingDistance( float fRimDistance, float fRimFalloffStart );

	RSCID CommitShaderMStage( RSCID* pBeforeComponent, bool bPointColor, bool bUseShade);
	RSCID CommitShaderGStage( RSCID* pBeforeComponent, bool bPointColor, bool bUseShade, float fDistanceToCamera );

	virtual void Update(RCameraSceneNode* pCamera) override;

private:
	bool		m_bAddColor;
	RVector3	m_vAddColor;

	bool		m_bGlowEffect;
	bool		m_bGlowEffectFading;
	RVector3	m_vGlowColor;
	float		m_fGlowScale;

	static bool	 m_bRimLighting;
	static float m_fRimDistance;		///< 림 라이트 적용 거리
	static float m_fRimFalloffStart;	///< 림 라이트 감쇠 시작 거리
	static float m_fRimFalloffLength;	///< 림 라이트 감쇠 적용 구간 거리 ( RimDistance - RimFalloffStart )
	static float m_fRimFalloffRate;		///< 림 라이트 감쇠 적용비 ( 1 / RimFalloffLength )
};

}