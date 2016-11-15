#include "StdAfx.h"
#include "RLODNode.h"
#include "RLODContext.h"


//////////////////////////////////////////////////////////////////////////
/// LOD¸¦ °ü¸®
//////////////////////////////////////////////////////////////////////////

namespace rs3
{

RLODNode::RLODNode() : m_pLODContext(NULL), m_nLODLevel(0)
{

}


RLODNode::~RLODNode()
{
	DeleteLODContext();
}

bool RLODNode::CreateLODContext( DWORD dwTypeFlags)
{
	if( !m_pLODContext)
	{
		m_pLODContext = new RLODContext(dwTypeFlags);
	}
	else
	{
		m_pLODContext->CreateProcessors(dwTypeFlags);
	}

	return true;
}

void RLODNode::DeleteLODContext()
{
	SAFE_DELETE( m_pLODContext);
}

bool RLODNode::SetMetricInfo( RLODMetricInfo* pInfo)
{
	if( m_pLODContext)
	{
		m_pLODContext->SetMetricInfo( pInfo);
		return true;
	}
	else
		return false;
	
}

int RLODNode::GetLODLevel()
{
	return m_nLODLevel;
}

void RLODNode::ResetLOD()	
{ 
	m_nLODLevel = 0; 
}

bool RLODNode::IsLODCullingPassed() const 
{ 
	return (m_nLODLevel >= 0); 
}

bool RLODNode::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	m_nLODLevel = 0;

	if( m_pLODContext)	
	{
		m_nLODLevel = m_pLODContext->CalculateLOD( pCamera, pBoundingSphere);
	}
	
	return true;
}




}
