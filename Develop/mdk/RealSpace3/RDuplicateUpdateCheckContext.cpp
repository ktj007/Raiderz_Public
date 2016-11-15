#include "stdafx.h"
#include "RDuplicateUpdateCheckContext.h"

namespace rs3
{

//////////////////////////////////////////////////////////////////////////
// RDuplicateUpdateCheckContext
RDuplicateUpdateCheckContext::RDuplicateUpdateCheckContext() :
 m_bNeedUpdate( false )
,m_nParentUpdatedID( 0 )
,m_nUpdatedID( 0 )
{
}

RDuplicateUpdateCheckContext::~RDuplicateUpdateCheckContext()
{
}

bool RDuplicateUpdateCheckContext::IsNeedUpdate( bool bNeedUpdateHierarchy, int nParentUpdatedID )
{
	// 변경 여부
	bool bNeedUpdate = m_bNeedUpdate;

	// 부모가 업데이트 되었다면 나도 업데이트 해야 한다
	if( bNeedUpdateHierarchy )
	{
		if( m_nParentUpdatedID != nParentUpdatedID )
		{
			bNeedUpdate = true;
			m_nParentUpdatedID = nParentUpdatedID;
		}
	}

	if( bNeedUpdate )
		m_nUpdatedID++;

	m_bNeedUpdate = false;

	return bNeedUpdate;
}

void RDuplicateUpdateCheckContext::SetForceUpdated( bool bNeedUpdateHierarchy, int nParentUpdatedID )
{
	if( bNeedUpdateHierarchy )
	{
		m_nParentUpdatedID = nParentUpdatedID;
	}

	m_nUpdatedID++;
	m_bNeedUpdate = false;
}

}