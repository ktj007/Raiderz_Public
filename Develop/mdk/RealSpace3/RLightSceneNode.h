#pragma once

#include "RSceneNode.h"


namespace rs3 {

/// 모든 광원의 base 클래스. 실제로 이 인스턴스를 사용하지는 않습니다.
class RS_API RLightSceneNode : public RSceneNode
{
	MDeclareRTTI;

//	DECLARE_USING_PASS(RRP_LIGHT);

public:
	RLightSceneNode(RRENDERPASS eRenderPass = RRP_LIGHT);
	virtual ~RLightSceneNode(void);

	bool GetEnable();

	void SetColorVector(RVector vColor) { m_color = vColor; }
	void SetColorRGB(DWORD dwColor);
	void SetAttenuationStart(float fAttenuationStart) { m_fAttenuationStart =fAttenuationStart; }
	
	RVector GetColorVector() const { return m_color; }
	DWORD GetColorRGB() const;
	float GetAttenuationStart() const { return m_fAttenuationStart; }	

	float GetApplyIntensity();

	bool CheckCameraInLight(RCameraSceneNode* pCamera);


	float	m_fIntensity;		// 로드되거나 초기화된 Intensity
	float	m_fApplyIntensity;	// 실제 적용되는 Intensity
	RVector	m_color;
	float	m_fAttenuationStart;
};

}