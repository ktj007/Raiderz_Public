#include "stdafx.h"
#include "RLODProcessorProjection.h"
#include "RLODMetricInfo.h"

namespace rs3
{

RLODProcessorProjection::RLODProcessorProjection()
{
	SetTypeID(LODP_PROJECTION);
}


RLODProcessorProjection::~RLODProcessorProjection()
{

}


int	RLODProcessorProjection::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	// 		float fArea = ( pCamera->GetNearZ() * pBoundingSphere->radius ) / fDistance;
	// 		float fAreaFactorFromCam = fArea / pCamera->GetNearHeight() / 2;
	// 	
	// 		RVector4 vP1 = pCamera->GetViewFrustum().GetPoints()[1];
	// 		RVector4 vP2 = pCamera->GetViewFrustum().GetPoints()[3];
	// 		RVector3 vP = RVector3( vP1.x, vP1.y, vP1.z) - RVector3( vP2.x, vP2.y, vP2.z);
	// 		float fPlength = vP.Length();
	// 		float fAreaFactorFromFrustum = fArea / fPlength / 2;
	// 		
	// 		if( fAreaFactorFromFrustum < DEFAULT_DISTANCE_CULL_CRITERION_FAR)
	// 		{
	// 			REngine::GetResourceProfiler().AddExcludedByDistanceCulling();
	// 			m_nLODResult = -1;
	// 			m_fFade = 0;
	// 			return fDistanceSphere;
	// 		}
	// 		else if( m_fDistanceCullingCriterionFar <= fArea && fArea < m_fDistanceCullingCriterionNear)
	// 		{
	// 			//REngine::GetResourceProfiler().AddExcludedByDistanceCulling();
	// 			m_nLODResult = 1;
	// 			m_fFade = ( fArea - m_fDistanceCullingCriterionFar) / (m_fDistanceCullingCriterionNear - m_fDistanceCullingCriterionFar);
	// 			return fDistanceSphere;
	// 		}

	float fDistance = pCamera->GetDistanceTo( pBoundingSphere->center );
	float fCriterionTangent = pCamera->GetDistanceCullingCriterionTangent();
	float fCriterionFarRadius = fDistance * fCriterionTangent;

	if( pBoundingSphere->radius < fCriterionFarRadius)
	{
		REngine::GetResourceProfiler().AddExcludedByDistanceCulling();
		return -1;
	}
	else
		return 0;
}



}