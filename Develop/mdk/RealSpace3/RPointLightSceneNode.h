#pragma once
#include "RLightSceneNode.h"

namespace rs3 {

RS_API extern const char *RSID_LIGHTSCENENODE;

/// actor 등에 영향을 줄 점광원
class RS_API RPointLightSceneNode : public RLightSceneNode
{
	MDeclareRTTI;
	DECLARE_NODE_ID_STRING(RSID_LIGHTSCENENODE);

public:
	RPointLightSceneNode(RRENDERPASS eRenderPass = RRP_LIGHT);
	virtual ~RPointLightSceneNode(void);

	virtual bool CreateFromXML(MXmlElement &element, bool bBackgroundCreation);
	virtual void SaveToXML(MXmlElement &element);

	virtual RSceneNode*	Clone();

	void SetRadius(float fRadius) { m_fRadius =fRadius; NeedUpdateBoundingVolume(); }
	float GetRadius() const { return m_fRadius; }

	void SetAreaRange(const RVector3& vRange) { _ASSERT( vRange >= RVector::ZERO ); m_vAreaRange =vRange; NeedUpdateBoundingVolume(); }
	RVector3 GetAreaRange() { return m_vAreaRange; }
	bool IsAreaLight()	{ return m_vAreaRange!=RVector3::ZERO; }
	ROBB GetOBB();

	float GetScale();

protected:
	virtual void OnUpdateAABB() override;
	virtual bool OnUpdateSphere() override;

private:
	void UpdateAABB();

	float		m_fRadius;
	RVector3	m_vAreaRange;
};

}