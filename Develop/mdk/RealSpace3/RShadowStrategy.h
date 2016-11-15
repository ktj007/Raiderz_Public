#pragma once

#include "RShaderComponentID.h"
#include "RProjectionShadow.h"
#include "RCameraSceneNode.h"

namespace rs3 {

class RActor;
class RTreeSceneNode;
class RDirectionalLightSceneNode;
class RPointLightSceneNode;
class RRenderingCallbackInterface;

enum RSHADOWMETHOD 
{
	SHADOW_NONE			= 0x00,
	SHADOW_LISPSM,
	SHADOW_PSSM,
	SHADOW_ESM,
	SHADOW_PROJECTION,

	SHADOW_END
};

enum RSHADOWFILTERMETHOD 
{
	SF_NONE = 0,
	SF_PCF,
	SF_POISSON,
	SF_PCSS,

	SF_END
};

static const int MAX_PSSM_SPLIT_COUNT = 4;

struct RS_API RShadowDesc
{
	RShadowDesc();

	bool operator == ( const RShadowDesc& other ) const
	{
		if( eMethod != other.eMethod ) return false;
		if( nTextureSize != other.nTextureSize ) return false;
		if( nTextureNum != other.nTextureNum ) return false;
		if( bDoubleShadow != other.bDoubleShadow ) return false;

		for( int i = 0; i < MAX_PSSM_SPLIT_COUNT; ++i )
		{
			if( fSplitDistance[i] != other.fSplitDistance[i] )
				return false;
		}

		return true;
	}

	// 공통 옵션
	RSHADOWMETHOD	eMethod;
	int				nTextureSize;
	bool			bDoubleShadow;

	// PSSM 옵션
	int				nTextureNum;
	float			fSplitDistance[MAX_PSSM_SPLIT_COUNT];
};

class RS_API RShadowStrategy
{
protected:
	bool				m_bShadowReceiveEnable;
	bool				m_bNeedHWFiltering;
	RShadowDesc			m_shadowDesc;

public:
	RShadowStrategy();

	virtual ~RShadowStrategy();

	virtual void Init() {}
	virtual void Destroy() {}
	virtual void ClearBuffers() {}		///< TODO: 필요한가 ?

	virtual void Render(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld) {}

	virtual RSCID GetDepthWriteComponent() { return RSCID(0,0); }

	virtual RTexture* GetCurrentDepthMap() { return NULL; }
	virtual RTexture* GetDoubleShadowDepthMap() { return NULL; }
	virtual int	GetCurrentDepthMapSize() { return 0; }
	virtual const RCameraSceneNode* GetCurrentDepthCamera() {	return NULL; }
	virtual const RMatrix GetCurrentLightTransform() { return RMatrix::IDENTITY; }
	virtual const RMatrix GetCurrentLightViewTransform() { return RMatrix::IDENTITY; }
	virtual RBoundingBox GetCurrentLightFrustumAABB()		{	return RBoundingBox(); }

	virtual bool IsCastShadow(const RMatrix& matView, const RMatrix& matViewProj, const RSphere& boundingSphere) { return true; }

	bool GetNeedHWFiltering()		{ return m_bNeedHWFiltering; }
	bool GetShadowReceiveEnabled()	{ return m_bShadowReceiveEnable; }
	const RShadowDesc& GetShadowDesc() const { return m_shadowDesc; }
};

}
