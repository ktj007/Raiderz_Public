#include "stdafx.h"
#include "RLODContext.h"
#include "RLODProcessorBase.h"
#include "RLODProcessorDistance.h"
#include "RLODProcessorFog.h"
#include "RLODProcessorProjection.h"

namespace rs3
{

RLODContext::RLODContext( DWORD dwTypeFlags)
{
	CreateProcessors( dwTypeFlags);
}


RLODContext::~RLODContext()
{
	DeleteProcessors();
}


bool RLODContext::CheckProcessor( DWORD dwType)
{
	for( int i=0; i<(int)m_vLodProcessBase.size(); i++)
	{
		if( m_vLodProcessBase[i]->GetTypeID() == dwType)
			return true;
	}

	return false;
}


bool RLODContext::CreateProcessors( DWORD dwTypeFlags)
{
	RPFC_THISFUNC;

	if( dwTypeFlags & LODP_DISTANCE)
	{
		if(!CheckProcessor(LODP_DISTANCE))
			m_vLodProcessBase.push_back( new RLODProcessorDistance());
	}
	if( dwTypeFlags & LODP_FOG)
	{
		if(!CheckProcessor(LODP_FOG))
			m_vLodProcessBase.push_back( new RLODProcessorFog());
	}
	if( dwTypeFlags & LODP_PROJECTION)
	{
		if(!CheckProcessor(LODP_PROJECTION))
			m_vLodProcessBase.push_back( new RLODProcessorProjection());
	}

	return true;
}


bool RLODContext::DeleteProcessors()
{
	RPFC_THISFUNC;

	for( int i=0; i<(int)m_vLodProcessBase.size(); i++)
	{
		SAFE_DELETE( m_vLodProcessBase[i]);
	}
	m_vLodProcessBase.clear();

	return true;
}

RLODProcessorBase* RLODContext::GetProcessor( DWORD dwType)
{
	RPFC_THISFUNC;

	for( int i=0; i<(int)m_vLodProcessBase.size(); i++)
	{
		if( (m_vLodProcessBase[i])->GetTypeID() == dwType)
			return m_vLodProcessBase[i];
	}

	return NULL;
}


bool RLODContext::SetMetricInfo( RLODMetricInfo* pInfo)
{
	RPFC_THISFUNC;

	/// 메트릭 인포는 디스턴스 컬링에서 사용된다
	RLODProcessorBase* pProcessor = GetProcessor( LODP_DISTANCE);
	if( pProcessor)
	{
		((RLODProcessorDistance*)pProcessor)->SetMetricInfo( pInfo);
		return true;
	}
	else
		return false;

}


/// LOD 레벨 상태를 반환한다.
/// 반환값
/// * -1 : 그리지 않는다.
/// * 0~n : 숫자가 낮을수록 하이폴리곤의 레벨 상태
int RLODContext::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	/// LOD 프로세서가 없으면 LOD 처리를 하지 않는다. 즉 최 상위 상태를 반환
	int nLowestLevel = 0;

	/// 프로세서들을 돌면서 제일 하위 상태의 레벨을 선택한다.
	for( int i=0; i<(int)m_vLodProcessBase.size(); i++)
	{
		int nLevel = m_vLodProcessBase[i]->CalculateLOD( pCamera, pBoundingSphere);
		if( nLevel < 0)
			return -1;
		else
			/// 숫자가 낮을수록 고퀄리티를 가리킨다. 헤깔리지 말아야 함.
			nLowestLevel = max( nLowestLevel, nLevel);
	}

	return nLowestLevel;
}


}