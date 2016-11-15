#pragma once

//#include "RLODProcessorBase.h"
#include <vector>
namespace rs3
{

class RLODProcessorBase;
class RLODMetricInfo;

class RS_API RLODContext
{
private:

public:
	RLODContext( DWORD dwTypeFlags);
	~RLODContext();

private:
	std::vector<RLODProcessorBase*>	m_vLodProcessBase;

public:
	bool	CreateProcessors( DWORD dwTypeFlags);
	bool	DeleteProcessors();
	bool	CheckProcessor( DWORD dwType);
	RLODProcessorBase*	GetProcessor( DWORD dwType);
	bool	SetMetricInfo( RLODMetricInfo* pInfo);
	int		CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere);
};
}