#include "stdafx.h"
#include "ROccluderContext.h"
#include "RRenderHelper.h"

namespace rs3
{
	ROccluderPointsSet::ROccluderPointsSet()
	{
	}

	ROccluderPointsSet::~ROccluderPointsSet()
	{
	}

	RVector3 ROccluderPoints::GetCenterPoint( void )
	{
		RVector3 CenterPoint(0,0,0);
		for(unsigned int i=0;i<GetNumberOfPoints();i++)
		{
			CenterPoint+= GetPoint(i);
		}
		CenterPoint/=(float)GetNumberOfPoints();
		return CenterPoint;
	}

	RAABB ROccluderPoints::GetAABB( void )
	{
		RAABB aabb;
		aabb.Initialize();
		for(unsigned int i=0;i<GetNumberOfPoints();i++)
		{
			aabb.Add(GetPoint(i));
		}
		return aabb;
	}

	RAABB ROccluderPointsSet::GetAABB( void )
	{
		RAABB aabb;
		aabb.Initialize();
		for(unsigned int i=0;i<GetTheNumberOfOccludePoints();i++)
		{
			ROccluderPoints &OccluderPoints = m_OccluderPointsSet[i];
			aabb.Add(OccluderPoints.GetAABB());
		}
		return aabb;
	}

	void ROccluderUtil::ConvertOccluderPointsToVariableFrusum(const RVector3& ViewCenter, ROccluderPoints& OccluderPoints,RVariableFrustum& VariableFrustum )
	{
		_ASSERT( 0 == VariableFrustum.GetNumberOfPlanes() );
		unsigned int size = OccluderPoints.GetNumberOfPoints();
		RVector3 center = OccluderPoints.GetCenterPoint();
		RPlane Plane;

		//³¯°³ ÇÁ·¯½ºÅÒ ¸¸µë
		for(unsigned int i=0;i<size;i++)
		{
			int index0;
			int index1;

			if( i == size-1 )
			{
				index0 = i;
				index1 = 0;
			}
			else
			{
				index0 = i;
				index1 = i+1;
			}

			const RVector3& p0 = OccluderPoints.GetPoint(index0);
			const RVector3& p1 = OccluderPoints.GetPoint(index1);
			RVector3 normalDir = center - p0;
			Plane.SetPlane(ViewCenter,p0,p1,normalDir);

			VariableFrustum.AddPlane(Plane);
		}

		//near Æò¸é ÇÁ·¯½ºÅÒ ¸¸µë
		Plane.SetPlane(OccluderPoints.GetPoint(0),OccluderPoints.GetPoint(1),center,OccluderPoints.GetPoint(0)-ViewCenter);
		VariableFrustum.AddPlane(Plane);
	}

	void ROccluderUtil::ConvertOccluderPointsSetToVariableFrustums( const RVector3& ViewCenter,ROccluderPointsSet& OccluderPointsSet,vector<RVariableFrustum>& VariableFrustums )
	{
		_ASSERT( 0 == VariableFrustums.size() );
		for(unsigned int i=0;i<OccluderPointsSet.GetTheNumberOfOccludePoints();i++)
		{
			RVariableFrustum VariableFrustum;
			ROccluderPoints& OccluderPoints = OccluderPointsSet.GetOCcluderPoints(i);
			ROccluderUtil::ConvertOccluderPointsToVariableFrusum(ViewCenter,OccluderPoints,VariableFrustum);
			VariableFrustums.push_back(VariableFrustum);
		}
	}	

	void ROccluderUtil::ConvertOccluderListToVariableFrustumsList( const RVector3& ViewCenter,ROccluderList& OccluderList,vector<vector<RVariableFrustum>>& VariableFrustumsList )
	{
		_ASSERT( 0 == VariableFrustumsList.size() );
		for(ROccluderList::ROCCLUDERPOINTSSET_LIST::iterator itor = OccluderList.m_OccluderList.begin();
			itor != OccluderList.m_OccluderList.end();
			itor++)
		{
			vector<RVariableFrustum>	Frustums;
			ROccluderPointsSet* pOccluderPointsSet = (*itor);
			ROccluderUtil::ConvertOccluderPointsSetToVariableFrustums(ViewCenter,*pOccluderPointsSet,Frustums);
			VariableFrustumsList.push_back(Frustums);
		}
	}

	void ROccluderUtil::RenderOcclusionPoints( ROccluderPoints& OccluderPoints,DWORD dwColor )
	{
		unsigned int size = OccluderPoints.GetNumberOfPoints();

		for(unsigned int i=0;i<size;i++)
		{
			int index0;
			int index1;

			if( i == size-1 )
			{
				index0 = i;
				index1 = 0;
			}
			else
			{
				index0 = i;
				index1 = i+1;
			}

			RVector3& p0 =(RVector3&) OccluderPoints.GetPoint(index0);
			RVector3& p1 =(RVector3&) OccluderPoints.GetPoint(index1);
			
			RRenderHelper::RenderLine(RMatrix::IDENTITY,p0,p1,dwColor);
		}
	}

	void ROccluderUtil::RenderOcclusionPointsSet( ROccluderPointsSet& OccluderPointsSet,DWORD dwColor /*= 0xffffffff*/ )
	{
		for( unsigned i=0;i<OccluderPointsSet.GetTheNumberOfOccludePoints();i++)
		{
			ROccluderUtil::RenderOcclusionPoints(OccluderPointsSet.GetOCcluderPoints(i),dwColor);
		}
	}
}