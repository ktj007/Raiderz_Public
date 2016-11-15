#pragma once

/**
 * Sky SceneNode : «œ¥√¿ª ∑ª¥ı∏µ «—¥Ÿ.
 */

#include "RTypes.h"
#include "RPType.h"
#include "RActor.h"
#include "RSceneNodeID.h"

namespace rs3 {		// begin rs3

class RMesh;
class RSkyManager;
struct RSkyInfo;
class RS_API RSceneSky : public RActor
{
	friend RSkyManager;

	DECLARE_NODE_ID_STRING(RSID_SKY);

protected:
	virtual void OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod) override;
	virtual int	 CalcPriority() override;

public:
	RSceneSky( RSkyInfo* pSkyInfo );
	virtual ~RSceneSky();

	virtual void OnAutoAnimationLoaded() override;

	virtual bool Create(const char *filename, bool bBackgroundCreation = false) override;

	// render sky box
	void Update();
	void Render( RCameraSceneNode* pCurrentCamera, const RVector4 &vMultiplyColor, float fogFactor, RRENDERINGMETHOD renderMothod);

private:
	RSkyInfo*	m_pSkyInfo;
};

}; // end rs3
