
#pragma once

#include "RShaderConstant.h"

namespace rs3 {

// 라이팅 셰이더를 생성하고, 셰이더 상수 설정을 하는 클래스
class RSpeedGrassShader
{
public:
	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};
	// Speed Grass Rendering Flag => SGRF
	static const DWORD SGRF_LIGHTBUFFER				= 1;
	static const DWORD SGRF_RT1						= 1<<1;		// Diffuse Spec 출력 RT
	static const DWORD SGRF_RT2						= 1<<2;
	static const DWORD SGRF_RT3						= 1<<3;
	
	static const FLAG_TABLE flagTable[];

public:
	RSpeedGrassShader();
	virtual ~RSpeedGrassShader();

	void		Init();
	void		Destroy();

	DWORD		GetShaderFlag( int nRenderTargetCount, bool bLightBuffer);
	RShaderFX*	CreateShader( DWORD dwFlag );

	void		SetTransformConstant(const RMatrix& matWorld, const RMatrix& matView, const RMatrix& matViewProj, const DWORD dwScreenWidth, const DWORD dwScreenHeight, const float fFarZ);;
	void		SetWindConstant( const float fNormalizedTime, const RVector3& vWindDirection, const float fWindStrength, const float fGrassJigglingPeriod, const float fGrassJigglingStrength, const float fGrassIncliningStrength);
	void		SetLodConstant( const float fLodFarDistance, const float fLodTransitionLength);
	void		SetBillboardConstant( const float* pUnitBillboard);

	RShaderFX*	GetShaderForGStageDeferred();
	RShaderFX*	GetShaderForGStageInferred();
	RShaderFX*	GetShaderForMStageInferred();

public:

	RShaderConstant			m_scWorldViewProj;
	RShaderConstant			m_scWorldView;
	RShaderConstant			m_scTimes;
	RShaderConstant			m_scWindStrength;
	RShaderConstant			m_scWindDirection;
	RShaderConstant			m_scBillboard;
	RShaderConstant			m_scFarLod;
	RShaderConstant			m_scLodTransition;
	RShaderConstant			m_scGrassJigglingPeriod;	// 떨림주기
	RShaderConstant			m_scGrassJigglingStrength;	// 떨림쎄기
	RShaderConstant			m_scGrassIncliningStrength;	// 기울여짐쎄기
	RShaderConstant			m_scTillingScale;
	RShaderConstant			m_scTillingOffset;
	RShaderConstant			m_scTillingVisibility;	

	RShaderConstant			m_scResolution;
	RShaderConstant			m_scFarZ;

	RShaderFXPool*	m_pShaderFXPool;
	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;
};

}