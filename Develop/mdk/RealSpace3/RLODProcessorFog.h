#pragma once

#include "RLODProcessorBase.h"

namespace rs3
{
class RS_API RLODProcessorFog : public RLODProcessorBase
{
private:

public:
	RLODProcessorFog();
	~RLODProcessorFog();

public:
	virtual int	CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) override;
};
}