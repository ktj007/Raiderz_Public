#pragma once

#include "RCameraSceneNode.h"
#include "RPassRenderer.h"
#include "RShaderConstant.h"
#include "RShaderFX.h"

namespace rs3
{

class RWaterPlane;
class RTexture;


class RS_API RWaterPassRenderer : public RPassRenderer
{
	friend class RWaterPlane;

public:
	RWaterPassRenderer();
	virtual ~RWaterPassRenderer();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// PassRenderer 구현
	//
	DECLARE_PASS_RENDERER_ID( RRP_WATER );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	virtual void			Init();
	virtual void			Destroy();
	virtual void			Reset();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 / 업데이트
	//
	virtual void			RenderMStageOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod) override;

	virtual bool			IsCurrentShaderRejective(){ return true; }
	virtual void			Update(float timegap);


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 렌더링 옵션
	//
	void					SetRenderDisable(bool b)	{ m_bRenderDisable = b; }


	void					RecreateBuffer();

private:
	void					RenderWaterPlaneMStage( RCameraSceneNode* cam, RWaterPlane* wp, int index );
	void					RenderMaskingMap(RWaterPlane* pWaterPlane);
	void					RenderResultToScreen();

	void					SetUnderWaterEffect( RCameraSceneNode* cam, SCENE_SET* pRenderingList );

	void					UploadShaderConst(RCameraSceneNode* pCamera, RRENDERINGMETHOD renderMethod);
	void					UploadShaderConstFromWPResource( RCameraSceneNode* pCurrentCamera, RWaterPlane* pWaterPlane);

	void					CreateVertexFormat();
	void					CreateShader();

	void					CreateBuffer();
	void					DestroyBuffer();


	RShaderFX*				m_pWaterShader;

	RTechnique				m_WaterTechnique;
	RTechnique				m_WaterTechniqueLow;
	RTechnique				m_UnderWaterTechnique;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	//쉐이더 상수
	RShaderConstant			m_scViewProjection;
	RShaderConstant			m_scViewInv;
	RShaderConstant			m_scRefractionTextureMat;
	RShaderConstant			m_scResolution;
	RShaderConstant			m_scViewProjInvMat;

	RShaderConstant			m_scFogColor;
	RShaderConstant			m_scFogParam;
	RShaderConstant			m_scBumpTexture1;
	RShaderConstant			m_scBumpTexture2;

	RShaderConstant			m_scTime;
	RShaderConstant			m_scCameraPos;

	RShaderConstant			m_scRefractionTexture;
	RShaderConstant			m_scDepthTexture;
	RShaderConstant			m_scMaskingTexture;
	RShaderConstant			m_scDepthBuffer;

	RShaderConstant			m_scBumpColorToVector;

	RShaderConstant			m_scDepthLimit;

	RShaderConstant			m_scBumpUVScale;

	RShaderConstant			m_scBumpMove1;
	RShaderConstant			m_scBumpMove2;

	RShaderConstant			m_scBumpAdjustFactor;
	RShaderConstant			m_scBumpWeight1;
	RShaderConstant			m_scBumpWeight2;
	RShaderConstant			m_scBumpWeight3;
	RShaderConstant			m_scBumpWeight4;
	RShaderConstant			m_scWaterRGBHalflife;
	RShaderConstant			m_scFresnelConstant;

	RShaderConstant			m_scReflectAdjust;

	RShaderConstant			m_scHalfVector;
	RShaderConstant			m_scDiffuseColor;

	RShaderConstant			m_scWaterMaterial;

	RShaderConstant			m_scFarClipCornerInView;
	RShaderConstant			m_scRefractAttenuation;


	// vertex wave 관련 쉐이더상수
	RShaderConstant			m_scScale;
	RShaderConstant			m_scAmplitude1;
	RShaderConstant			m_scPhase1;
	
	//Shore 관련
	RShaderConstant			m_scShoreTexture;
	RShaderConstant			m_scShoreInfo;
	

	//////////////////////////////////////////////////////////////////////////

	HVERTEXFORMAT			m_hVertexFormat;			
	vector<RWaterPlane*>	m_vRenderableWaterPlanes;

	float					m_fTime;

	bool					m_bRenderDisable;

	RListener*				m_pEventListener;

	RTexture*				m_pRefractionMap;
	RTexture*				m_pMakingMap;
};

}