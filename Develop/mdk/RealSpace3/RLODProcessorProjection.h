#pragma once

#include "RLODProcessorBase.h"

namespace rs3
{

class RS_API RLODProcessorProjection : public RLODProcessorBase
{
private:
	

public:
	RLODProcessorProjection();
	~RLODProcessorProjection();

public:
	virtual int	CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) override;
};

}