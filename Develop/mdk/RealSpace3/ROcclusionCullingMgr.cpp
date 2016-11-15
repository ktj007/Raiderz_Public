#include "stdafx.h"
#include "ROcclusionCullingMgr.h"

namespace rs3
{
	ROcclusionCullingMgr::ROcclusionCullingMgr( void )
	{
		m_pCullingCallBack = new ROcclusionCullingCallBack(this);
	}

	ROcclusionCullingMgr::~ROcclusionCullingMgr( void )
	{
	}

	const int ROcclusionCullingMgrSet::NUM_OCCLUSION_CULLING_MGR = 10;

	ROcclusionCullingMgrSet::ROcclusionCullingMgrSet( void )
		:m_pOcclusionCullingMgrs(NULL),m_UsedOcclusionCullingMgrIndex(0),m_TheNumberOfOccludedSceneNodes(0),m_CallOrderValidator(OCMS_RESETOCCLUDEDSCENENODES)
	{
		m_pOcclusionCullingMgrs = new ROcclusionCullingMgr[NUM_OCCLUSION_CULLING_MGR];
	}

	ROcclusionCullingMgrSet::~ROcclusionCullingMgrSet( void )
	{
		SAFE_DELETE_ARRAY(m_pOcclusionCullingMgrs);
	}

	void ROcclusionCullingMgrSet::OcclusionCull(RWorldSceneNode* pWorldScene, const RVector3& ViewPosition ,ROccluderList& OccluderList)
	{
		RPFC_THISFUNC;

		_ASSERT( OCMS_RESETOCCLUDEDSCENENODES == m_CallOrderValidator);
		m_CallOrderValidator = OCMS_OCCLUSIONCULL;

		m_UsedOcclusionCullingMgrIndex = 0;
		_ASSERT(0 == m_UsedOcclusionCullingMgrIndex);

		vector<vector<RVariableFrustum>> FrustumsList;
		ROccluderUtil::ConvertOccluderListToVariableFrustumsList(ViewPosition,OccluderList,FrustumsList);

		unsigned int IndexOcclusionCullingMgr		= 0;
		
		for( unsigned int i=0;i<FrustumsList.size() && IndexOcclusionCullingMgr< GetTheNumberOfOcclusionMgrs() ;i++ )
		{
			vector<RVariableFrustum>& Frustums = FrustumsList[i];
			for(unsigned int j=0;j<Frustums.size() && IndexOcclusionCullingMgr < GetTheNumberOfOcclusionMgrs() ;j++)
			{
				RCullingMgr& OcclusionCullingMgr = m_pOcclusionCullingMgrs[IndexOcclusionCullingMgr];

				int planes = Frustums[j].GetNumberOfPlanes();
				RPlane* pPlanes = new RPlane[planes];
				for(int planeI=0;planeI<planes;planeI++)
				{
					pPlanes[planeI] = Frustums[j].GetPlane(planeI);
				}

				OcclusionCullingMgr.OcclusionCull(pWorldScene,pPlanes,planes);

				delete[] pPlanes;

				IndexOcclusionCullingMgr++;
			}
		}

		m_UsedOcclusionCullingMgrIndex = IndexOcclusionCullingMgr;
	}

	void	ROcclusionCullingMgrSet::SetOccludedSceneNodes( void )
	{
		RPFC_THISFUNC;

		_ASSERT( OCMS_OCCLUSIONCULL == m_CallOrderValidator);
		m_CallOrderValidator = OCMS_SETOCCLUDEDSCENENODES;

		int TheNumberOfOccludedSceneNodes = 0;
		for( int i=0;i<m_UsedOcclusionCullingMgrIndex ;i++ )
		{
			RCullingMgr& OcclusionCullingMgr = m_pOcclusionCullingMgrs[i];
			for( int pass = RRP_DIRLIGHT; pass <=RRP_SPOTLIGHT; ++pass )
			{
				if( IsEceptionalPass(pass) ) continue;
				SCENE_SET *pOccuSceneSet = OcclusionCullingMgr.GetCulledList((RRENDERPASS)pass);
				for(SCENE_SET::iterator iter = pOccuSceneSet->begin() ; iter != pOccuSceneSet->end() ; iter++ )
				{
					RSceneNode* pNode = (*iter);
					pNode->SetOcclued(true); 
					TheNumberOfOccludedSceneNodes++;
				}
			}
		}
		m_TheNumberOfOccludedSceneNodes = TheNumberOfOccludedSceneNodes;
	}

	void ROcclusionCullingMgrSet::ResetOccludedSceneNodes(void)
	{
		RPFC_THISFUNC;

		_ASSERT( OCMS_SETOCCLUDEDSCENENODES == m_CallOrderValidator);
		m_CallOrderValidator = OCMS_RESETOCCLUDEDSCENENODES;

		for( int i=0;i<m_UsedOcclusionCullingMgrIndex ;i++ )
		{
			RCullingMgr& OcclusionCullingMgr = m_pOcclusionCullingMgrs[i];
			for( int pass = RRP_DIRLIGHT; pass <=RRP_SPOTLIGHT; ++pass )
			{
				if( IsEceptionalPass(pass) ) continue;
				SCENE_SET *pOccuSceneSet = OcclusionCullingMgr.GetCulledList((RRENDERPASS)pass);
				for(SCENE_SET::iterator iter = pOccuSceneSet->begin() ; iter != pOccuSceneSet->end() ; iter++ )
				{
					RSceneNode* pNode = (*iter);
					pNode->SetOcclued(false); m_TheNumberOfOccludedSceneNodes--;
				}
			}
		}

		_ASSERT( 0 == m_TheNumberOfOccludedSceneNodes );
		m_UsedOcclusionCullingMgrIndex = 0;
		m_TheNumberOfOccludedSceneNodes = 0;
	}

	bool ROcclusionCullingMgrSet::IsEceptionalPass( int pass )
	{
		if( pass == RRP_SKYBOX )	return true;
		return false;
	}
}