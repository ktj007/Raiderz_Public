#include "stdafx.h"
#include "RLODProcessorDistance.h"
#include "RLODMetricInfo.h"

namespace rs3
{
RLODProcessorDistance::RLODProcessorDistance()
{
	SetTypeID(LODP_DISTANCE);
}

RLODProcessorDistance::~RLODProcessorDistance()
{

}


bool RLODProcessorDistance::SetMetricInfo( RLODMetricInfo* pMetricInfo)
{
	m_pLODMetricInfoRef = pMetricInfo;
	return true;
}

int	RLODProcessorDistance::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	if(m_pLODMetricInfoRef)
	{
		/// GetDistanceTo()의 연산 비용 : *:3 +:4 -:1
		float fDistanceSphere = pCamera->GetDistanceTo( *pBoundingSphere );

		int nMetricListSize = m_pLODMetricInfoRef->GetMetricListSize();
		int i=0;
		for( i=0; i<nMetricListSize; i++)
		{
			float fMetric = m_pLODMetricInfoRef->GetMetric(i);
			if( fDistanceSphere <= fMetric)
				break;
		}
		/// 매트릭 사이즈를 넘으면 그 넘는 인덱스를 레벨로 돌려준다.
		return i;
	}

	return 0;
}





}