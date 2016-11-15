#include "stdafx.h"
#include "RViewFrustumCullingMgr.h"

namespace rs3
{
	RViewFrustumCullingMgr::RViewFrustumCullingMgr( void )
	{
		m_pCullingCallBack = new RCullingCallBack(this);
		SetRenderingList<RRenderingList>();
	}

	RViewFrustumCullingMgr::~RViewFrustumCullingMgr( void )
	{
	}
}