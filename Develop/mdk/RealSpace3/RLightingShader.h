#pragma once

#include "RShaderConstant.h"

namespace rs3 {

// 라이팅 셰이더를 생성하고, 셰이더 상수 설정을 하는 클래스
class RLightingShader
{
public:
	// Dir Lighting Flag => DLF
	static const DWORD DLF_DIR_LIGHT				= 1;
	static const DWORD DLF_LIGHTING					= 1<<1;	// 정의되지 않으면 Ambient lighting만 한다.
	static const DWORD DLF_SPECULAR					= 1<<2;
	static const DWORD DLF_RIM						= 1<<3;
	static const DWORD DLF_CLOUD					= 1<<4;
	static const DWORD DLF_CLOUD2					= 1<<5;
	static const DWORD DLF_CALC_SHADOW				= 1<<6;
	static const DWORD DLF_SHADOW_PSSM				= 1<<7;	// SHADOW 플래그가 켜져 있어야 동작.
	static const DWORD DLF_SHADOW_LERP_SPLIT		= 1<<8;	// SHADOW 플래그가 켜져 있어야 동작.
	static const DWORD DLF_SHADOW_FILTER_POISSON	= 1<<9;	// SHADOW 플래그가 켜져 있어야 동작.
	static const DWORD DLF_EXPORT_SHADOW_ONLY		= 1<<10;
	static const DWORD DLF_IMPORT_SHADOW			= 1<<11;
	static const DWORD DLF_IMPORT_GI				= 1<<12;

	static const DWORD DLF_POINT_LIGHT				= 1<<13;
	static const DWORD DLF_AREA_LIGHT				= 1<<14;

	static const DWORD DLF_SPOT_LIGHT				= 1<<15;
	static const DWORD DLF_SPOT_INSTANCING			= 1<<16; // 스팟 옵션
	static const DWORD DLF_SPOT_MAP					= 1<<17; // 스팟 옵션

	static const DWORD DLF_PROJECTION_SHADOW		= 1<<18;

	static const DWORD DLF_SUB_LIGHT_COUNT_1		= 1<<19;
	static const DWORD DLF_SUB_LIGHT_COUNT_2		= 1<<20;
	static const DWORD DLF_SUB_LIGHT_COUNT_3		= 1<<21;
	static const DWORD DLF_SUB_LIGHT_COUNT_4		= 1<<22;

	static const DWORD DLF_SUB_LIGHT_ONLY			= 1<<23;

	static const DWORD DLF_SKY_SPECULAR				= 1<<24;
	static const DWORD DLF_GAMMA_CORRECT			= 1<<25;

public:
	RLightingShader();
	virtual ~RLightingShader();

	void		Init();
	void		Destroy();

	void		Resize( int nW, int nH ) {}
	void		Invalidate() {}
	void		Restore() {}

	// TODO: 렌더 하는 곳으로 옮겨가야 함
	void		RenderQuad( RTechnique* pTech, DWORD dwW, DWORD dwH);

	RShaderFX*	CreateDirectionalLightShader( RDirectionalLightSceneNode* pDirLight, bool bSpecular, bool bRim, bool bExportShadow, bool bImportShadow, bool bImportGI, bool bFilter, bool bLerp, int nSubLightCount);
	RShaderFX*	CreateShader( DWORD dwFlag );
	RShaderFX*	CreateShaderForPointLight(bool bSpecular);
	RShaderFX*	CreateShaderForAreaLight(bool bSpecular);
	RShaderFX*	CreateShaderForSpotLightInstancing(bool bSpecular);
	RShaderFX*	CreateShaderForSpotLight( bool bShadow, bool bSpotmap, bool bSpecular);
	RShaderFX*	CreateShaderForShadowReceiver();
	RShaderFX*	CreateShaderForSubLight(int nCount);

public:
	RShaderConstant	m_scWorldViewProj;
	RShaderConstant	m_scWorldView;
	RShaderConstant	m_scFarClipCornerInView;
	RShaderConstant	m_scResolution;
	RShaderConstant	m_scLightSizeUV;
	RShaderConstant	m_scLightFrustumDepth;
	RShaderConstant	m_scLightTransform;
	RShaderConstant	m_scLightViewTransform;
	RShaderConstant	m_scLightTransformArray;
	RShaderConstant	m_scLightViewTransformArray;
	RShaderConstant	m_scDepthTexelSize;
	RShaderConstant	m_scCSMSplitInfo;
	RShaderConstant	m_scSplitCount;
	RShaderConstant	m_scDepthBias;
	RShaderConstant	m_scCloudFactor;
	RShaderConstant	m_scCloudTransform;
	RShaderConstant	m_scCloudFactor2;
	RShaderConstant	m_scCloudTransform2;
	RShaderConstant	m_scLightVec;
	RShaderConstant	m_scLightColor;
	RShaderConstant	m_scLightAmbient;
	RShaderConstant	m_scShadowLuminosity;
	RShaderConstant	m_scSubLightColor;
	RShaderConstant	m_scSubLightDir;
	RShaderConstant	m_scLightPosition;
	RShaderConstant	m_scAttenuationInfo;
	RShaderConstant	m_scVisibility;
	RShaderConstant	m_scLightDirectionFOV;
	RShaderConstant	m_scLightPositionArray;
	RShaderConstant	m_scLightColorArray;
	RShaderConstant	m_scLightInfoArray;
	RShaderConstant	m_scLightDirectionFOVArray;
	RShaderConstant	m_scSkySpecularTerm;
	RShaderConstant	m_scSubLightIntensity;

private:
	struct FLAG_TABLE
	{
		const DWORD dwFlag;
		char* strDefine;
	};

	static const FLAG_TABLE flagTable[];

	RShaderFXPool*	m_pShaderFXPool;

	typedef map<DWORD, RShaderFX*> SHADERLIST;
	SHADERLIST		m_ShaderList;
};

}