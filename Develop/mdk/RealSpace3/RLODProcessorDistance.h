#pragma once

#include "RLODProcessorBase.h"

namespace rs3
{

class RLODMetricInfo;

class RS_API RLODProcessorDistance : public RLODProcessorBase
{
private:
	RLODMetricInfo* m_pLODMetricInfoRef;	/// 자체적으로 소유하는 것이 아니라 포인터 참조이다.

public:
	RLODProcessorDistance();
	~RLODProcessorDistance();

public:
	bool SetMetricInfo( RLODMetricInfo* pMetricInfo);

	virtual int	CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere) override;
};
}