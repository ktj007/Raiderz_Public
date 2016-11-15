#pragma once

#include "RPointLightSceneNode.h"

namespace rs3 {

class CORETOOL_API RToolPointLightSceneNode : public RPointLightSceneNode
{
public:
	RToolPointLightSceneNode(CControlMediator* pControlMediator);
	virtual RSceneNode* Clone();

protected:

	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMethod = RENDERING_NULL) override;
	virtual bool OnPick(RPICKINFO& pickInfo);

	CControlMediator*	m_pControlMediator;
};

}
