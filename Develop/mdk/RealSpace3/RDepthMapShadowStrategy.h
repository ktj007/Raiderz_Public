#pragma once

#include "RShadowStrategy.h"
namespace rs3 {

//그림자맵 재사용 기법을 사용하기 시작하는 그림자맵 구간
//이 구간부터 재사용 기법 사용
#define REUSE_MIN_INDEX 2		

class RS_API RDepthMapShadowStrategy : public RShadowStrategy
{
public:
	RDepthMapShadowStrategy();
	virtual ~RDepthMapShadowStrategy();

	virtual RSCID GetDepthWriteComponent();

	virtual RTexture* GetCurrentDepthMap() { return m_pRenderTarget; }
	virtual RTexture* GetDoubleShadowDepthMap() { return m_pDoubleShadowRT; }
	virtual int	GetCurrentDepthMapSize() { return m_nTextureSize; }
	
	virtual bool IsCastShadow(const RMatrix& matView, const RMatrix& matViewProj, const RSphere& boundingSphere);

	RShaderFX* GetClearFloatShader()	{ return m_pShaderClearFloat; }

	void RenderShadowMap(RCameraSceneNode& shadowCamera);
	void CombineDoubleShadowMap();

	//DepthMap 재사용 기법 관련
	RTexture* GetReusedDepthMap(int nIndex)	{ return m_pReusedRT[nIndex]; }
	void SetResetDepthMap(bool bReset)	{ m_bResetDepthMap = bReset; }
	void CombineDepthMap(int nDepthMapCount);

	//Debug 용
	void RenderDepthMapToScreen(int nX, int nY, int nWidth, int nHeight);

	void SetUseRenderMinArea(bool bEnable)	{ m_bUseRenderMinArea = bEnable; }
	void SetRenderMinArea(float fMinArea)	{ m_fMinRenderArea = fMinArea; }

	bool GetUseRenderMinArea()	{ return m_bUseRenderMinArea; }
	float GetRenderMinArea()	{ return m_fMinRenderArea; }

	RVector4 GetDepthBias() { return m_vDepthBias; }

protected:

	void RenderShadowMap(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld, RCameraSceneNode &shadowCamera, RShadowTransform* ShdowTransform, 
							float fNearZ, float fFarZ, const RVector3 &vLightDirection);

	bool CreateClearFX();
	void DestroyClearFX();

	/// 깊이값을 최대값으로 채워넣는다
	void ClearDepth(int x, int y, int nWidth, int nHeight);

	virtual void SetandClearRenderTarget(RCameraSceneNode* pCamera, RTexture* pRenderTarget);

	void MakeTextureScaleTransform();

	int				m_nTextureSize;			///< 렌더타켓 버퍼의 텍스쳐 사이즈
	float			m_fBorderSize;			///< 사용하지 않는 영역, 테두리1 을 줘서 늘어지는 그림자를 막는다
	RShaderFX*		m_pShaderClearFloat;	///< float 로 clear 할수 있는 쉐이더
	RMatrix			m_matTextureScale;		///< 라이트 뷰프로젝션 공간 -> 텍스쳐 공간
	RMatrix			m_matToler;				///< 그림자 인정하는 최소 오차

	RPlane			m_debugPlane;			///< 디버깅용 평면

	RTexture*		m_pDepthBuffer;

	RVector4		m_vDepthBias;

	//-------------------------------------------------
	//DepthMap 재사용 기법 관련
	void SetRenderTargetIndex(int nPrevRT, int nCurrentRT)	{ m_nPrevRT=nPrevRT; m_nCurrentRT=nCurrentRT; }
	
	void RenderReusedShadowMap(RCameraSceneNode *pViewCamera, RWorldSceneNode *pWorld, RCameraSceneNode* shadowCamera, RShadowTransform* ShdowTransform, 
		float fNearZ, float fFarZ);

	bool							m_bResetDepthMap;
	int								m_nPrevRT;
	int								m_nCurrentRT;
	RTexture*						m_pRenderTarget;
	RTexture*						m_pCommonShadowRT;
	RTexture*						m_pDoubleShadowRT;
	RTexture**						m_pReusedRT;
	RDirectionalLightSceneNode*		m_pDirectionalLight;
	RVector3						m_vLightDirection;
	//-------------------------------------------------

private:
	void	CullCasters( RWorldSceneNode* pWorld, RCameraSceneNode &shadowCamera );

	void	FindMinMaxZObject(RCameraSceneNode &shadowCamera, RRENDERPASS beginPass, RRENDERPASS endPass, float& fOutmin, float& fOutmax);

	//-----------------------------------------------------------------------------------------
	//DepthMap 재사용 기법 관련
	void SetStaticPass(RCameraSceneNode* pCamera);
	void SetDynamicPass(RCameraSceneNode* pCamera);

	bool CullUpdatedRegion(RWorldSceneNode* pWorld, RCameraSceneNode* pCamera, RViewFrustum* pPrevFrustum, RVector3 vCameraDelta,
							RRENDERPASS BeginPass, RRENDERPASS EndPass);
	
	void RenderStaticSceneNode(RCameraSceneNode* pCamera, RWorldSceneNode* pWorld, RShadowTransform* pShadowTranform, RVector3 vPrevLightPos, RMatrix matPrevViewProj);
	void RenderPrevDepthMap(const RMatrix& matPrevViewProj, const RMatrix& matCurrentViewProj);
	void RenderDynamicSceneNode(RWorldSceneNode *pWorld, RCameraSceneNode* pCamera, RShadowTransform* pShadowTranform);
	void RenderSceneNode(RCameraSceneNode* pCamera, RRENDERPASS beginPass, RRENDERPASS endPass);
	void RenderDoubleShadowSceneNode(RCameraSceneNode* pCamera);

	void GetCullPlanesForX(const RViewFrustum* pPrevFrustum, const RViewFrustum* pCurrentFrustum, float fDelta, RPlane* outPlanes);
	void GetCullPlanesForY(const RViewFrustum* pPrevFrustum, const RViewFrustum* pCurrentFrustum, float fDelta, RPlane* outPlanes);
	//-----------------------------------------------------------------------------------------

	RSceneNodeUpdateVisitor m_ShadowCameraUpdator;

	bool m_bUseRenderMinArea;
	float m_fMinRenderArea;
};

}