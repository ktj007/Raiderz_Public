#pragma once

#include "RShaderConstant.h"

namespace rs3 {

// 라이팅 셰이더를 생성하고, 셰이더 상수 설정을 하는 클래스
class RTerrainShader
{
public:
	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};
	// Terrain Rendering Flag => TRF
	static const DWORD TRF_SPECULAR					= 1;
	static const DWORD TRF_RT1						= 1<<1;		// Diffuse Spec 출력 RT
	static const DWORD TRF_RT2						= 1<<2;
	static const DWORD TRF_RT3						= 1<<3;
	static const DWORD TRF_BAKED					= 1<<4;
	static const DWORD TRF_LIGHTBUFFER				= 1<<5;		// 라이트 버퍼로부터 라이팅
	static const DWORD TRF_FOG						= 1<<6;
	static const DWORD TRF_SPLATTING_ONEPASS		= 1<<7;
	static const DWORD TRF_LAYER_COUNT_2			= 1<<8;
	static const DWORD TRF_LAYER_COUNT_3			= 1<<9;
	static const DWORD TRF_LAYER_COUNT_4			= 1<<10;
	static const DWORD TRF_LAYER_COUNT_5			= 1<<11;
	static const DWORD TRF_LAYER_COUNT_6			= 1<<12;
	static const DWORD TRF_LAYER_COUNT_7			= 1<<13;
	static const DWORD TRF_LAYER_0_SPECULAR			= 1<<14;
	static const DWORD TRF_LAYER_1_SPECULAR			= 1<<15;
	static const DWORD TRF_LAYER_2_SPECULAR			= 1<<16;
	static const DWORD TRF_LAYER_3_SPECULAR			= 1<<17;
	static const DWORD TRF_LAYER_4_SPECULAR			= 1<<18;
	static const DWORD TRF_LAYER_5_SPECULAR			= 1<<19;
	static const DWORD TRF_LAYER_6_SPECULAR			= 1<<20;
	static const DWORD TRF_LAYER_0_NORMALMAP		= 1<<21;
	static const DWORD TRF_LAYER_1_NORMALMAP		= 1<<22;
	static const DWORD TRF_LAYER_2_NORMALMAP		= 1<<23;
	static const DWORD TRF_LAYER_3_NORMALMAP		= 1<<24;
	static const DWORD TRF_LAYER_4_NORMALMAP		= 1<<25;
	static const DWORD TRF_LAYER_5_NORMALMAP		= 1<<26;
	static const DWORD TRF_LAYER_6_NORMALMAP		= 1<<27;
	static const DWORD TRF_USE_NORMALMAP			= 1<<28;
	static const DWORD TRF_GRID						= 1<<29;
	static const DWORD TRF_MARKINGLYER				= 1<<30;
	

	static const FLAG_TABLE flagTable[];

public:
	RTerrainShader();
	virtual ~RTerrainShader();

	void		Init();
	void		Destroy();

	DWORD		GetShaderFlag( RDirectionalLightSceneNode* pDirLight, bool bExportShadow, bool bImportShadow, bool bImportGI, bool bFilter, bool bLerp);
	DWORD		GetRenderTargetCountFlag( int nMRT);
	DWORD		GetLayerCountFlag( int nLayerCount);
	RShaderFX*	CreateShader( DWORD dwFlag );
	RShaderFX*	GetShaderForBakedMapGStage( int nMRT, bool bSpecular);
	RShaderFX*	GetShaderForGrid( int nMRT, bool bSpecular);
	RShaderFX*	GetShaderForBakedMapMStage( bool bLighting = true);
	RShaderFX*	GetShaderForSplattingLayerOnePass( bool bLighting, int nLayer, int nRenderTargetCount, int* pSpecularArray, int* pNormalmapFlagArray, bool bMarkingLayer);
	RShaderFX*	GetShaderForMakeMask();

public:
	RShaderConstant	m_scFogFactor;
	RShaderConstant	m_scFogColor;
	RShaderConstant	m_scWorld;
	RShaderConstant	m_scWorldViewProj;
	RShaderConstant	m_scWorldView;
	RShaderConstant	m_scTextureTransform;
	RShaderConstant	m_scTileTextureScaleAndTranslate;
	RShaderConstant	m_scAlphaTextureScaleAndTranslate;
	RShaderConstant	m_scFarDist;
	RShaderConstant	m_scResolution;
	RShaderConstant	m_scLayerCount;
	/// One-pass Splatting시 사용
	RShaderConstant	m_scTileTextureScaleAndTranslateArray;
	/// Marking에서 사용
	RShaderConstant	m_csMarkinglayer;						

	RShaderFX*		m_pFxMakeMask;	// one-pass용 마스크 텍스쳐 만드는 테크닉
	RTechnique		m_techMakeMask;	// one-pass용 마스크 텍스쳐 만드는 테크닉

	RShaderFXPool*	m_pShaderFXPool;
	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;
};

}