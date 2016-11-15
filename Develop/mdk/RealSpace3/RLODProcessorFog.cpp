#include "stdafx.h"
#include "RLODProcessorFog.h"

namespace rs3
{

RLODProcessorFog::RLODProcessorFog()
{
	SetTypeID(LODP_FOG);
}


RLODProcessorFog::~RLODProcessorFog()
{

}

int	RLODProcessorFog::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	if(!pCamera->GetFogEnable())
		return 0;

	/// GetDistanceTo()의 연산 비용 : *:3 +:4 -:1
	float fDistanceSphere = pCamera->GetDistanceTo( *pBoundingSphere );
	if( fDistanceSphere > pCamera->GetFogFar())
	{
		REngine::GetResourceProfiler().AddExcludedByFogCulling();
		return -1;	
	}
	else
		return 0;
}


}