#pragma once
#include "RSceneNodeID.h"
#include "RLightSceneNode.h"
#include "RCameraSceneNode.h"
#include "RLiSPSMShadowStrategy.h"

namespace rs3 {

#define DEFAULT_SPOATMAP "Sunlight.dds"

class RS_API RSpotLightSceneNode : public RLightSceneNode
{
	MDeclareRTTI;

	DECLARE_NODE_ID_STRING(RSID_SPOTLIGHTSCENENODE);

//	DECLARE_USING_PASS(RRP_SPOTLIGHT);

public:
	RSpotLightSceneNode(void);
	virtual ~RSpotLightSceneNode(void);

	void RenderShadowMap(RCameraSceneNode* pViewCamera);
	void ShowShadowMapToScreen(int posX, int posY, int nSize);


	RShadowStrategy* GetShadowStrategy()	{ return m_pShadowStrategy; }
	RTexture* GetDepthMap() { return m_pShadowStrategy->GetCurrentDepthMap(); }

	const RMatrix GetLightTransform()	{ return m_pShadowStrategy->GetCurrentLightTransform(m_LightCamera); }
	const RMatrix GetLightViewTransform()	{ return m_LightCamera.GetViewMatrix(); }

	RTexture* GetSpotLightTexture()	{ return m_hSpotLightTexture; }
	const string& GetSpotLightTextureFileName() { return m_strSpotLightTextureFileName; }
	float GetFar()	{ return m_fFar; }
	float GetFov()	{ return m_fFov; }
	RViewFrustum GetLightFrustum()	{ return m_LightCamera.GetViewFrustum(); }
	bool GetShadowCast()	{ return m_bShadowCast; }
	bool GetUseRenderMinArea()	{ return m_pShadowStrategy->GetUseRenderMinArea(); }
	float GetRenderMinArea()	{ return m_pShadowStrategy->GetRenderMinArea(); }
	
	void SetSpotLightTexture(const char* strFilePath);
	void SetFar(float fFar)	{ m_fFar = fFar; NeedUpdateBoundingVolume(); }
	void SetFov(float fFov)	{ m_fFov = fFov; NeedUpdateBoundingVolume(); }
	void SetShadowCast(bool bShadowCast);
	void SetUseRenderMinArea(bool bEnable)	{ m_pShadowStrategy->SetUseRenderMinArea(bEnable); }
	void SetRenderMinArea(float fMinArea)	{ m_pShadowStrategy->SetRenderMinArea(fMinArea); }

	bool IsInViewFrustum(RCameraSceneNode* pCamera);

protected:
	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void SaveToXML(MXmlElement &element);

	virtual RSceneNode* Clone();

	virtual void OnUpdate() override;
	virtual void OnUpdateAABB() override;

private:
	void UpdateAABB();

	RSphere GetBoundingSphere();

	RCameraSceneNode m_LightCamera;

	RExponentialShadowStrategy* m_pShadowStrategy;

	string m_strSpotLightTextureFileName;
	RTexture* m_hSpotLightTexture;

	float m_fFar;
	float m_fFov;
	bool m_bShadowCast;
};

}
